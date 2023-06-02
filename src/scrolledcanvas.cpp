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

#include <iostream>
#include "scrolledcanvas.h"
#include "canvasitem.h"
#include "defs.h"
#include "scaler.h"
#include <wx/dcclient.h>
#include <wx/defs.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/scrolbar.h>

ScrolledCanvas::ScrolledCanvas(wxWindow *parent, wxWindowID id) : wxWindow(parent, id) {
    layout = new wxFlexGridSizer(2, 2, 0, 0);
    canvas = new wxWindow(this, wxID_ANY);
    vBar = new wxScrollBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);
    hBar = new wxScrollBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
    layout->AddGrowableCol(0);
    layout->AddGrowableRow(0);
    layout->Add(canvas, 1, wxEXPAND);
    hBar->SetScrollbar(0, 50, 100, 50);
    vBar->SetScrollbar(0, 50, 100, 50);
    layout->Add(vBar, 1, wxEXPAND);
    layout->Add(hBar, 1, wxEXPAND);
    SetSizer(layout);
    scaler = new Scaler(1, 1, ict::FLOOR_ST);
    testRectangle = new CanvasItem(wxRect(0, 0, 40, 40), nullptr, scaler, false);
    canvas->Bind(wxEVT_PAINT, &ScrolledCanvas::paintCanvas, this);
    canvas->Bind(wxEVT_MOTION, &ScrolledCanvas::mouseMotion, this);
    canvas->Bind(wxEVT_LEFT_DOWN, &ScrolledCanvas::mousePress, this);
    canvas->Bind(wxEVT_LEFT_UP, &ScrolledCanvas::mouseRelease, this);
}

void ScrolledCanvas::mouseMotion(wxMouseEvent &event) {
    if(testRectangle->zonePressed() != ict::NONE) {
        wxRect oldRect = testRectangle->getGeometry();
        if(testRectangle->modify(event.GetPosition()))
            canvas->RefreshRect(testRectangle->getGeometry().Union(oldRect));
    }
    event.Skip();
}

void ScrolledCanvas::mousePress(wxMouseEvent &event) {
    if(!canvas->HasCapture()) canvas->CaptureMouse();
    ict::ItemZone pz = testRectangle->press(event.GetPosition());
    event.Skip();
}

void ScrolledCanvas::mouseRelease(wxMouseEvent &event) {
    if(canvas->HasCapture()) canvas->ReleaseMouse();
    if(testRectangle->zonePressed() != ict::NONE) testRectangle->release();
    event.Skip();
}

void ScrolledCanvas::paintCanvas(wxPaintEvent &event) {
    wxPaintDC painter(canvas);
    wxRegion damaged = canvas->GetUpdateRegion();
    wxRect toPaint(damaged.GetBox());
    painter.SetBrush(*wxBLACK_BRUSH);
    painter.DrawRectangle(toPaint);
    toPaint = testRectangle->getGeometry().Intersect(toPaint);
    painter.SetBrush(*wxRED_BRUSH);
    painter.DrawRectangle(toPaint);
}

ScrolledCanvas::~ScrolledCanvas() {

}
