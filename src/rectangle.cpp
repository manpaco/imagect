#include "rectangle.h"

wxIMPLEMENT_DYNAMIC_CLASS(Rectangle, wxControl);

Rectangle::Rectangle() {
    Init();
}

Rectangle::Rectangle(wxWindow *parent, wxWindowID id, 
                const wxPoint &pos, 
                const wxSize &size, long style, 
                const wxValidator &validator, 
                const wxString &name) {
    Init();
    Create(parent, id, pos, size, style, validator, name);
}

void Rectangle::Init() {
    Bind(wxEVT_MOTION, &Rectangle::mouseMotion, this);
    Bind(wxEVT_LEFT_DOWN, &Rectangle::mousePress, this);
    Bind(wxEVT_LEFT_UP, &Rectangle::mouseRelease, this);
    Bind(wxEVT_LEAVE_WINDOW, &Rectangle::resetCursor, this);
}

void Rectangle::resetCursor(wxMouseEvent &event) {
    wxSetCursor(wxNullCursor);
}

void Rectangle::mouseMotion(wxMouseEvent &event) {
    if(innerZone(event.GetPosition())) {
        wxSetCursor(wxCursor(wxCURSOR_CROSS));
    } else {
        changeCursor(edgeZone(event.GetPosition()));
    }
    if(drag) {
        // move
    } else {
        // resize
    }
    event.Skip();
}

void Rectangle::changeCursor(ict::CardinalPoint type) {
    switch (type) {
        case ict::N | ict::S:
            // set nort-south arrow cursor
            wxSetCursor(wxCursor(wxCURSOR_SIZENS));
            break;
        case ict::W | ict::E:
            // set west-east arrow cursor
            wxSetCursor(wxCursor(wxCURSOR_SIZEWE));
            break;
    }
}

void Rectangle::mousePress(wxMouseEvent &event) {
    if(innerZone(event.GetPosition())) drag = true;
    else resize = edgeZone(event.GetPosition());
}

void Rectangle::mouseRelease(wxMouseEvent &event) {
    drag = false;
    resize = ict::NONE;
}

bool Rectangle::innerZone(const wxPoint p) {
    wxRect zone(30, 30, 170, 170);
    if(isContained(zone, p)) return true;
    return false;
}

ict::CardinalPoint Rectangle::edgeZone(const wxPoint p) {
    if(innerZone(p)) return ict::NONE;
    wxRect nz(0, 0, 50, 50);
    if(isContained(nz, p)) return ict::N;
    return ict::NONE;
}

bool Rectangle::isContained(wxRect area, wxPoint point) {
    if((area.GetX() <= point.x && point.x <= area.GetWidth()) &&
        (area.GetY() <= point.y && point.y <= area.GetHeight())) return true;
    return false;
}

void Rectangle::OnPaint(wxPaintEvent &) {
    wxPaintDC pencil(this);
    pencil.DrawRectangle(9, 0, 100, 100);
}

Rectangle::~Rectangle() {

}
