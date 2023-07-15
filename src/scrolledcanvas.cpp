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
#include "scrolledcanvas.h"
#include "canvasitem.h"
#include "defs.h"
#include "scaler.h"
#include <wx/dcmemory.h>
#include <wx/dcclient.h>
#include <wx/defs.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/scrolbar.h>
#include <wx/utils.h>

ScrolledCanvas::ScrolledCanvas(wxWindow *parent, wxWindowID id) : wxWindow(parent, id) {
    layout = new wxFlexGridSizer(2, 2, 0, 0);
    canvas = new wxWindow(this, wxID_ANY);
    canvas->SetBackgroundStyle(wxBG_STYLE_PAINT);
    vBar = new wxScrollBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);
    hBar = new wxScrollBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
    scaler = new Scaler(1.0, 1.0, ict::FLOOR_ST);
    canvasBuffer = nullptr;
    pressItem = nullptr;
    oldSelectedItem = nullptr;
    referenceItem = new CanvasItem(-1, wxRect(0, 0, 10, 10));
    referenceItem->setScaler(scaler);
    zOrder.push_back(referenceItem);
    xMagError = 0.0;
    yMagError = 0.0;
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
    canvas->Bind(wxEVT_PAINT, &ScrolledCanvas::paintCanvas, this);
    canvas->Bind(wxEVT_MOTION, &ScrolledCanvas::mouseMotion, this);
    canvas->Bind(wxEVT_LEFT_DOWN, &ScrolledCanvas::mousePress, this);
    canvas->Bind(wxEVT_LEFT_UP, &ScrolledCanvas::mouseRelease, this);
    canvas->Bind(wxEVT_SIZE, &ScrolledCanvas::canvasResize, this);
    canvas->Bind(wxEVT_MOUSEWHEEL, &ScrolledCanvas::magnification, this);
}

void ScrolledCanvas::magnification(wxMouseEvent &event) {
    int wheelRotation = event.GetWheelRotation();
    if (wheelRotation > 0) scaler->addFactor(0.3, 0.3);
    else scaler->addFactor(-0.3, -0.3);
    doMagnify(event.GetPosition());
    //doMagnify(wxPoint(0, 0));
    event.Skip();
}

void ScrolledCanvas::canvasResize(wxSizeEvent &event) {
    if (canvasBuffer) delete canvasBuffer;
    canvasBuffer = new wxBitmap(event.GetSize().GetWidth(), event.GetSize().GetHeight());
    refreshCanvas();
}

void ScrolledCanvas::mouseMotion(wxMouseEvent &event) {
    if(pressItem) if(pressItem->modify(event.GetPosition())) refreshCanvas();
    event.Skip();
}

CanvasItem * ScrolledCanvas::pressCanvas(const wxPoint p) {
    for (std::vector<CanvasItem *>::reverse_iterator it = zOrder.rbegin(); it != zOrder.rend(); it++) {
        if ((*it)->press(p) != ict::NONE) return *it;
    }
    return nullptr;
}

void ScrolledCanvas::mousePress(wxMouseEvent &event) {
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

void ScrolledCanvas::mouseRelease(wxMouseEvent &event) {
    if(canvas->HasCapture()) canvas->ReleaseMouse();
    if(pressItem) {
        pressItem->release();
        pressItem = nullptr;
    }
    event.Skip();
}

void ScrolledCanvas::paintCanvas(wxPaintEvent &event) {
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

CanvasItem * ScrolledCanvas::getItem(int itemId) {
    for (std::vector<CanvasItem *>::iterator it = zOrder.begin(); it != zOrder.end(); it++) {
        if ((*it)->getId() == itemId) return *it;
    }
    return nullptr;
}

void ScrolledCanvas::addItem(CanvasItem *item) {
    if (!item) return;
    if (getItem(item->getId())) return;
    item->setScaler(scaler);
    item->setCanvasReference(referenceItem);
    zOrder.push_back(item);
    refreshCanvasRect(item->getArea());
}

void ScrolledCanvas::doMagnify(const wxPoint mousePosition) {
    if (scaler && !scaler->hasTransfer()) return;
    if (mousePosition.x < 0 || mousePosition.y < 0) return;

    std::cout << "mp: " << mousePosition.x << " - " << mousePosition.y << std::endl;
    double of, tf, nf;
    scaler->getOldFactor(&of, nullptr);
    scaler->getTransferFactor(&tf, nullptr);
    scaler->getNewFactor(&nf, nullptr);
    double xip, yip;
    xip = (referenceItem->getX(VIRTUAL_CONTEXT) + std::floor(xMagError)) * of;
    yip = (referenceItem->getY(VIRTUAL_CONTEXT) + std::floor(yMagError)) * of;
    std::cout << "ip: " << xip << " - " << yip << std::endl;
    xip -= mousePosition.x;
    yip -= mousePosition.y;
    xip = xip * tf;
    yip = yip * tf;
    xip += mousePosition.x;
    yip += mousePosition.y;
    std::cout << "nip: " << xip << " - " << yip << std::endl;
    xip = xip / nf;
    yip = yip / nf;
    xMagError = xip - std::floor(xip);
    yMagError = yip - std::floor(yip);
    std::cout << "vnip: " << xip << " - " << yip << std::endl;
    referenceItem->setVirtualPosition(wxPoint(xip, yip));
    std::cout << "errors: " << xMagError << " - " << yMagError << std::endl;
    std::cout << "-------------------------------" << std::endl;

    scaler->clearTransfer();
    refreshCanvas();
}

void ScrolledCanvas::doScroll(const wxPoint motion) {
    if (motion.x == 0 && motion.y == 0) return;

    refreshCanvas();
}

void ScrolledCanvas::refreshCanvasRect(const wxRect &r) {
    canvas->RefreshRect(r);
}

void ScrolledCanvas::refreshCanvas() {
    canvas->Refresh();
}

ScrolledCanvas::~ScrolledCanvas() {
    if (canvasBuffer) delete canvasBuffer;
}
