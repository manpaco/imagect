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
}

void Rectangle::mouseMotion(wxMouseEvent &event) {
    if(innerZone(event.GetPosition())) {
        // change cursor pointer to cross pointer
    } else {
        // change cursor pointer to arrow pointer
        switch (edgeZone(event.GetPosition())) {
            case ict::N:
                // set nort arrow cursor pointer
                break;
            case ict::S:
                // set south arrow cursor pointer
                break;
        }
    }
    if(drag) {
        // move
    } else {
        // resize
    }
    event.Skip();
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
}

bool Rectangle::isContained(wxRect area, wxPoint point) {
    if((area.GetX() <= point.x && area.GetWidth() >= point.x)) return true;
    return false;
}

void Rectangle::OnPaint(wxPaintEvent &) {
    wxPaintDC pencil(this);
    pencil.DrawRectangle(9, 0, 100, 100);
}

Rectangle::~Rectangle() {

}
