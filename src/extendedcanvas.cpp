/**
 *     Copyright (C) 2023 Acuilán Gabriel <acuilangabriel@gmail.com>
 *
 *     This file is part of ImageCT.
 *
 *     ImageCT is free software: you can redistribute it and/or modify it under
 *     the terms of the GNU General Public License as published by the Free
 *     Software Foundation, either version 3 of the License, or (at your
 *     option) any later version.
 *
 *     ImageCT is distributed in the hope that it will be useful, but WITHOUT
 *     ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *     FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *     more details.
 *
 *     You should have received a copy of the GNU General Public License along
 *     with ImageCT. If not, see <https://www.gnu.org/licenses/>.
 */

#include <cmath>
#include <math.h>
#include <iostream>
#include "extendedcanvas.hpp"
#include "canvasitem.hpp"
#include "defs.hpp"
#include "scaler.hpp"
#include <wx/dcmemory.h>
#include <wx/dcclient.h>
#include <wx/defs.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/scrolbar.h>
#include <wx/utils.h>

ExtendedCanvas::ExtendedCanvas(wxWindow *parent, wxWindowID id) : wxWindow(parent, id) {
    layout = new wxFlexGridSizer(2, 2, 0, 0);
    canvas = new wxWindow(this, wxID_ANY);
    canvas->SetBackgroundStyle(wxBG_STYLE_PAINT);
    canvasReference = wxPoint2DDouble(0, 0);
    vBar = new wxScrollBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);
    hBar = new wxScrollBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
    scaler = new Scaler(1.0, 1.0);
    canvasBuffer = nullptr;
    pressItem = nullptr;
    oldSelectedItem = nullptr;
    zoom = new wxWindow(this, wxID_ANY);
    zoom->SetBackgroundColour(wxColour(*wxYELLOW));
    layout->AddGrowableCol(0);
    layout->AddGrowableRow(0);
    layout->Add(canvas, 1, wxEXPAND);
    hBar->SetScrollbar(0, 50, 100, 50);
    vBar->SetScrollbar(0, 50, 100, 50);
    layout->Add(vBar, 1, wxEXPAND);
    layout->Add(hBar, 1, wxEXPAND);
    layout->Add(zoom, 1, wxEXPAND);
    SetSizer(layout);
    canvas->Bind(wxEVT_PAINT, &ExtendedCanvas::paintCanvas, this);
    canvas->Bind(wxEVT_MOTION, &ExtendedCanvas::mouseMotion, this);
    canvas->Bind(wxEVT_LEFT_DOWN, &ExtendedCanvas::mousePress, this);
    canvas->Bind(wxEVT_LEFT_UP, &ExtendedCanvas::mouseRelease, this);
    canvas->Bind(wxEVT_SIZE, &ExtendedCanvas::resizeCanvas, this);
    canvas->Bind(wxEVT_MOUSEWHEEL, &ExtendedCanvas::magnification, this);
}

void ExtendedCanvas::magnification(wxMouseEvent &event) {
    int wheelRotation = event.GetWheelRotation();
    if (wheelRotation > 0) scaler->plusFactor(0.3, 0.3);
    else scaler->plusFactor(-0.3, -0.3);
    doMagnify(event.GetPosition());
    //doMagnify(wxPoint(0, 0));
    event.Skip();
}

void ExtendedCanvas::resizeCanvas(wxSizeEvent &event) {
    if (canvasBuffer) delete canvasBuffer;
    canvasBuffer = new wxBitmap(event.GetSize().GetWidth(), event.GetSize().GetHeight());
    refreshCanvas();
}

void ExtendedCanvas::mouseMotion(wxMouseEvent &event) {
    if(pressItem) pressItem->modify(event.GetPosition());
    event.Skip();
}

CanvasItem * ExtendedCanvas::pressCanvas(const wxPoint p) {
    for (std::vector<CanvasItem *>::reverse_iterator it = zOrder.rbegin(); it != zOrder.rend(); it++) {
        if ((*it)->press(p)) return *it;
    }
    return nullptr;
}

void ExtendedCanvas::mousePress(wxMouseEvent &event) {
    if(!canvas->HasCapture()) canvas->CaptureMouse();
    pressItem = pressCanvas(event.GetPosition());
    if (!oldSelectedItem) {
        if (pressItem) refreshCanvas();
    } else if (!pressItem) {
        oldSelectedItem->select(false);
        refreshCanvas();
    } else if (*pressItem != *oldSelectedItem) {
        oldSelectedItem->select(false);
        refreshCanvas();
    }
    oldSelectedItem = pressItem;
    event.Skip();
}

void ExtendedCanvas::mouseRelease(wxMouseEvent &event) {
    if(canvas->HasCapture()) canvas->ReleaseMouse();
    if(pressItem) {
        pressItem->release();
        pressItem = nullptr;
    }
    event.Skip();
}

void ExtendedCanvas::paintCanvas(wxPaintEvent &event) {
    if (!canvasBuffer) return;
    wxPaintDC canvasPainter(canvas);
    wxRegion damaged = canvas->GetUpdateRegion();
    wxRect toPaint(damaged.GetBox());
    {
    wxMemoryDC bufferPainter(*canvasBuffer);
    bufferPainter.SetBrush(*wxBLACK_BRUSH);
    bufferPainter.DrawRectangle(toPaint);
    for (std::vector<CanvasItem *>::iterator it = zOrder.begin(); it != zOrder.end(); it++) {
        (*it)->drawOn(&bufferPainter);
    }
    }
    canvasPainter.DrawBitmap(*canvasBuffer, 0, 0);
}

CanvasItem * ExtendedCanvas::getItem(int itemId) {
    for (std::vector<CanvasItem *>::iterator it = zOrder.begin(); it != zOrder.end(); it++) {
        if ((*it)->getId() == itemId) return *it;
    }
    return nullptr;
}

void ExtendedCanvas::addItem(CanvasItem *item) {
    if (!item) return;
    if (getItem(item->getId())) return;
    item->setScaler(scaler);
    item->setContainer(this);
    zOrder.push_back(item);
    wxRect2DDouble fresh(item->getArea());
    refreshCanvasRect(wxRect(fresh.m_x, fresh.m_y, fresh.m_width, fresh.m_height));
}

void ExtendedCanvas::doMagnify(const wxPoint mousePosition) {
    if (scaler && !scaler->hasTransfer()) return;
    if (mousePosition.x < 0 || mousePosition.y < 0) return;

    double of, tf, nf;
    scaler->getOldFactor(&of, nullptr);
    scaler->getTransferFactor(&tf, nullptr);
    scaler->getNewFactor(&nf, nullptr);
    double xip, yip;
    xip = canvasReference.m_x * of;
    yip = canvasReference.m_y * of;
    xip -= mousePosition.x;
    yip -= mousePosition.y;
    xip = xip * tf;
    yip = yip * tf;
    xip += mousePosition.x;
    yip += mousePosition.y;
    xip = xip / nf;
    yip = yip / nf;
    canvasReference.m_x = xip;
    canvasReference.m_y = yip;

    scaler->clearTransfer();
    refreshCanvas();
}

void ExtendedCanvas::doScroll(const wxPoint motion) {
    if (motion.x == 0 && motion.y == 0) return;

    refreshCanvas();
}

void ExtendedCanvas::refreshCanvasRect(const wxRect &r) {
    canvas->RefreshRect(r);
}

void ExtendedCanvas::refreshCanvas() {
    canvas->Refresh();
}

void ExtendedCanvas::notifyChange(CanvasItem *changed) {
    wxRect2DDouble ch(changed->getUpdateArea());
    wxRect refresh(ch.m_x, ch.m_y, ch.m_width, ch.m_height);
    refreshCanvasRect(refresh);
}

wxPoint2DDouble ExtendedCanvas::getReference(ict::ECContext c) const {
    if(c == ict::VIRTUAL_CONTEXT) return canvasReference;
    else return scaler->scalePoint(canvasReference, ict::IN_D);
}

ExtendedCanvas::~ExtendedCanvas() {
    if (canvasBuffer) delete canvasBuffer;
}
