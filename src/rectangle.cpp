#include "rectangle.h"
#include <iterator>

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
    Bind(wxEVT_PAINT, &Rectangle::OnPaint, this);
    Bind(wxEVT_SIZE, &Rectangle::updateSizes, this);
}

void Rectangle::updateSizes(wxSizeEvent &event) {
    ratio = event.GetSize().GetWidth() / event.GetSize().GetHeight();
    iz = wxRect(corner, corner, event.GetSize().GetWidth() - (corner*2), 
            event.GetSize().GetHeight() - (corner*2));
    nz = wxRect(corner, 0, event.GetSize().GetWidth() - (corner * 2), corner);
    sz = wxRect(corner, event.GetSize().GetHeight() - corner, 
            event.GetSize().GetWidth() - (corner * 2), corner);
    ez = wxRect(event.GetSize().GetWidth() - corner, corner, corner, 
            event.GetSize().GetHeight() - (corner * 2));
    wz = wxRect(0, corner, corner, event.GetSize().GetHeight() - (corner * 2));
    nez = wxRect(event.GetSize().GetWidth() - corner, 0, corner, corner);
    nwz = wxRect(0, 0, corner, corner);
    sez = wxRect(event.GetSize().GetWidth() - corner, 
            event.GetSize().GetHeight() - corner, corner, corner);
    swz = wxRect(0, event.GetSize().GetHeight() - corner, corner, corner);
    event.Skip();
}

void Rectangle::resetCursor(wxMouseEvent &event) {
    if(!zonePressed) changeCursor(ict::NONE);
}

void Rectangle::mouseMotion(wxMouseEvent &event) {
    if(!zonePressed) changeCursor(getLocation(event.GetPosition()));
    if(zonePressed == ict::INNER) {
        wxPoint screenPoint(wxGetMousePosition());
        screenPoint = screenPoint - clientPressPoint;
        Move(GetParent()->ScreenToClient(screenPoint));
    } else {
        resizeUsing(zonePressed);
    }
}

void Rectangle::resizeUsing(ict::Zone zone){
    wxPoint mousePosition(wxGetMousePosition());
    wxPoint thisPosition(GetScreenPosition());
    int limitPosX = thisPosition.x + resizeLimit;
    int limitPosY = thisPosition.y + resizeLimit;
    int deltaX = 0, deltaY = 0;
    if(zone == ict::SE) {
        deltaX = mousePosition.x - (thisPosition.x + GetSize().GetWidth());
        deltaY = mousePosition.y - (thisPosition.y + GetSize().GetHeight());
        deltaX++;
        deltaY++;
        if(mousePosition.x < limitPosX && mousePosition.y < limitPosY) return;
        if(mousePosition.x < limitPosX) {
            SetSize(wxDefaultCoord, wxDefaultCoord, wxDefaultCoord, GetSize().GetHeight() + deltaY, wxSIZE_USE_EXISTING); 
            return;
        }
        if(mousePosition.y < limitPosY) {
            SetSize(wxDefaultCoord, wxDefaultCoord, GetSize().GetWidth() + deltaX, wxDefaultCoord, wxSIZE_USE_EXISTING); 
            return;
        }
        SetSize(wxDefaultCoord, wxDefaultCoord, GetSize().GetWidth() + deltaX, GetSize().GetHeight() + deltaY);
    }
}

void Rectangle::changeCursor(ict::Zone type) {
    if(type == ict::N || type == ict::S) 
        wxSetCursor(wxCursor(wxCURSOR_SIZENS));
    if(type == ict::E || type == ict::W) 
        wxSetCursor(wxCursor(wxCURSOR_SIZEWE));
    if(type == ict::NE || type == ict::SW) 
        wxSetCursor(wxCursor(wxCURSOR_SIZENESW));
    if(type == ict::NW || type == ict::SE) 
        wxSetCursor(wxCursor(wxCURSOR_SIZENWSE));
    if(type == ict::INNER)
        wxSetCursor(wxCursor(wxCURSOR_HAND));
    if(type == ict::NONE)
        wxSetCursor(wxNullCursor);
}

void Rectangle::mousePress(wxMouseEvent &event) {
    CaptureMouse();
    clientPressPoint = event.GetPosition();
    zonePressed = getLocation(event.GetPosition());
}

void Rectangle::mouseRelease(wxMouseEvent &event) {
    ReleaseMouse();
    zonePressed = ict::NONE;
}

ict::Zone Rectangle::getLocation(const wxPoint p) {
    if(isContained(nz, p)) return ict::N;
    if(isContained(sz, p)) return ict::S;
    if(isContained(ez, p)) return ict::E;
    if(isContained(wz, p)) return ict::W;
    if(isContained(nez, p)) return ict::NE;
    if(isContained(nwz, p)) return ict::NW;
    if(isContained(sez, p)) return ict::SE;
    if(isContained(swz, p)) return ict::SW;
    if(isContained(iz, p)) return ict::INNER;
    return ict::NONE;
}

bool Rectangle::isContained(wxRect area, wxPoint point) {
    int rightSide = area.GetWidth() + area.GetX();
    int bottomSide = area.GetHeight() + area.GetY();
    if((area.GetX() < point.x && point.x < rightSide) &&
        (area.GetY() < point.y && point.y < bottomSide)) return true;
    return false;
}

void Rectangle::OnPaint(wxPaintEvent &) {
    wxPaintDC device(this);
    device.SetPen(wxPen(wxColor("red"), borderWidth));
    device.DrawRectangle(1, 1, GetSize().GetWidth()-1, 
            GetSize().GetHeight()-1);
}

Rectangle::~Rectangle() {

}
