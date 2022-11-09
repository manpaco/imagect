#include "rectangle.h"
#include <iterator>
#include <wx/defs.h>
#include <wx/gdicmn.h>
#include <wx/geometry.h>
#include <wx/graphics.h>

wxIMPLEMENT_DYNAMIC_CLASS(Rectangle, wxControl);

Rectangle::Rectangle() {
    init();
}

Rectangle::Rectangle(wxWindow *parent, wxWindowID id, 
                const wxPoint &pos, 
                const wxSize &size, long style, 
                const wxValidator &validator, 
                const wxString &name) {
    init();
    Create(parent, id, pos, size, style, validator, name);
}

void Rectangle::init() {
    Bind(wxEVT_MOTION, &Rectangle::mouseMotion, this);
    Bind(wxEVT_LEFT_DOWN, &Rectangle::mousePress, this);
    Bind(wxEVT_LEFT_UP, &Rectangle::mouseRelease, this);
    Bind(wxEVT_LEAVE_WINDOW, &Rectangle::leaveWinHandler, this);
    Bind(wxEVT_ENTER_WINDOW, &Rectangle::enterWinHandler, this);
    Bind(wxEVT_PAINT, &Rectangle::onPaint, this);
    Bind(wxEVT_SIZE, &Rectangle::updateSizes, this);
}

void Rectangle::enterWinHandler(wxMouseEvent &event) {
    mouseLeftWin = false;
}

void Rectangle::updateSizes(wxSizeEvent &event) {
    ratio = (float)event.GetSize().GetWidth() / 
            (float)event.GetSize().GetHeight();
    iz = wxRect((GetSize().GetWidth() / 2) - (dragWidth / 2), 
            (GetSize().GetHeight() / 2) - (dragWidth / 2), 
            dragWidth, dragWidth);
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

void Rectangle::leaveWinHandler(wxMouseEvent &event) {
    if(!zonePressed) changeCursor(ict::NONE);
    mouseLeftWin = true;
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
    wxPoint positionOnScreen(GetScreenPosition());
    wxPoint positionOnParent(GetPosition());
    int limitPosX = 0;
    int limitPosY = 0;
    int deltaX = 0, deltaY = 0;
    if(zone == ict::SE) {
        limitPosX = positionOnScreen.x + resizeLimit;
        limitPosY = positionOnScreen.y + resizeLimit;
        deltaX = corner / 2;
        deltaY = corner / 2;
        deltaX += mousePosition.x - 
                (positionOnScreen.x + GetSize().GetWidth());
        deltaY += mousePosition.y - 
                (positionOnScreen.y + GetSize().GetHeight());
        if(mousePosition.x < limitPosX && mousePosition.y < limitPosY) return;
        if(mousePosition.x < limitPosX) {
            SetSize(wxDefaultCoord, wxDefaultCoord, wxDefaultCoord, 
                    GetSize().GetHeight() + deltaY, wxSIZE_USE_EXISTING); 
            return;
        }
        if(mousePosition.y < limitPosY) {
            SetSize(wxDefaultCoord, wxDefaultCoord, 
                    GetSize().GetWidth() + deltaX, wxDefaultCoord, 
                    wxSIZE_USE_EXISTING); 
            return;
        }
        SetSize(wxDefaultCoord, wxDefaultCoord, GetSize().GetWidth() + deltaX, 
                GetSize().GetHeight() + deltaY);
        return;
    }
    if(zone == ict::NW) {
        limitPosX = (positionOnScreen.x + GetSize().GetWidth()) - resizeLimit;
        limitPosY = (positionOnScreen.y + GetSize().GetHeight()) - resizeLimit;
        deltaX = -corner / 2;
        deltaY = -corner / 2;
        deltaX += mousePosition.x - positionOnScreen.x;
        deltaY += mousePosition.y - positionOnScreen.y;
        if(mousePosition.x > limitPosX && mousePosition.y > limitPosY) return;
        if(mousePosition.x > limitPosX) {
            SetSize(wxDefaultCoord, positionOnParent.y + deltaY, 
                    wxDefaultCoord, GetSize().GetHeight() - deltaY, 
                    wxSIZE_USE_EXISTING);
            return;
        }
        if(mousePosition.y > limitPosY) {
            SetSize(positionOnParent.x + deltaX, wxDefaultCoord, 
                    GetSize().GetWidth() - deltaX, wxDefaultCoord, 
                    wxSIZE_USE_EXISTING);
            return;
        }
        SetSize(positionOnParent.x + deltaX, positionOnParent.y + deltaY, 
                GetSize().GetWidth() - deltaX, GetSize().GetHeight() - deltaY);
        return;
    }
    if(zone == ict::NE) {
        limitPosX = positionOnScreen.x + resizeLimit;
        limitPosY = (positionOnScreen.y + GetSize().GetHeight()) - resizeLimit;
        deltaX = corner / 2;
        deltaY = -corner / 2;
        deltaX += mousePosition.x - 
                (positionOnScreen.x + GetSize().GetWidth());
        deltaY += mousePosition.y - positionOnScreen.y;
        if(mousePosition.x < limitPosX && mousePosition.y > limitPosY) return;
        if(mousePosition.x < limitPosX) {
            SetSize(wxDefaultCoord, positionOnParent.y + deltaY, 
                    wxDefaultCoord, GetSize().GetHeight() - deltaY, 
                    wxSIZE_USE_EXISTING);
            return;
        }
        if(mousePosition.y > limitPosY) {
            SetSize(wxDefaultCoord, wxDefaultCoord, 
                    GetSize().GetWidth() + deltaX, wxDefaultCoord, 
                    wxSIZE_USE_EXISTING);
            return;
        }
        SetSize(wxDefaultCoord, positionOnParent.y + deltaY, 
                GetSize().GetWidth() + deltaX, 
                GetSize().GetHeight() - deltaY, wxSIZE_USE_EXISTING);
        return;
    }
    if(zone == ict::SW) {
        limitPosX = (positionOnScreen.x + GetSize().GetWidth()) - resizeLimit;
        limitPosY = positionOnScreen.y + resizeLimit;
        deltaX = -corner / 2;
        deltaY = corner / 2;
        deltaX += mousePosition.x - positionOnScreen.x;
        deltaY += mousePosition.y - 
                (positionOnScreen.y + GetSize().GetHeight());
        if(mousePosition.x > limitPosX && mousePosition.y < limitPosY) return;
        if(mousePosition.x > limitPosX) {
            SetSize(wxDefaultCoord, wxDefaultCoord, wxDefaultCoord, 
                    GetSize().GetHeight() + deltaY, wxSIZE_USE_EXISTING);
            return;
        }
        if(mousePosition.y < limitPosY) {
            SetSize(positionOnParent.x + deltaX, wxDefaultCoord, 
                    GetSize().GetWidth() - deltaX, wxDefaultCoord, 
                    wxSIZE_USE_EXISTING);
            return;
        }
        SetSize(positionOnParent.x + deltaX, wxDefaultCoord, 
                GetSize().GetWidth() - deltaX, GetSize().GetHeight() + deltaY, 
                wxSIZE_USE_EXISTING);
        return;
    }
    if(zone == ict::N) {
        limitPosY = (positionOnScreen.y + GetSize().GetHeight()) - resizeLimit;
        deltaY = -corner / 2;
        deltaY += mousePosition.y - positionOnScreen.y;
        if(mousePosition.y > limitPosY) return;
        SetSize(wxDefaultCoord, positionOnParent.y + deltaY, wxDefaultCoord, 
                GetSize().GetHeight() - deltaY, wxSIZE_USE_EXISTING);
        return;
    }
    if(zone == ict::S) {
        limitPosY = positionOnScreen.y + resizeLimit;
        deltaY = corner / 2;
        deltaY += mousePosition.y - 
                (positionOnScreen.y + GetSize().GetHeight());
        if(mousePosition.y < limitPosY) return;
        SetSize(wxDefaultCoord, wxDefaultCoord, wxDefaultCoord, 
                GetSize().GetHeight() + deltaY, wxSIZE_USE_EXISTING);
        return;
    }
    if(zone == ict::W) {
        limitPosX = (positionOnScreen.x + GetSize().GetWidth()) - resizeLimit;
        deltaX = -corner / 2;
        deltaX += mousePosition.x - positionOnScreen.x;
        if(mousePosition.x > limitPosX) return;
        SetSize(positionOnScreen.x + deltaX, wxDefaultCoord, 
                GetSize().GetWidth() - deltaX, wxDefaultCoord, 
                wxSIZE_USE_EXISTING);
        return;
    }
    if(zone == ict::E) {
        limitPosX = positionOnScreen.x + resizeLimit;
        deltaX = corner / 2;
        deltaX += mousePosition.x - 
                (positionOnScreen.x + GetSize().GetWidth());
        if(mousePosition.x < limitPosX) return;
        SetSize(wxDefaultCoord, wxDefaultCoord, 
                GetSize().GetWidth() + deltaX, wxDefaultCoord, 
                wxSIZE_USE_EXISTING);
        return;
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
    if(mouseLeftWin) changeCursor(ict::NONE);
    else changeCursor(getLocation(event.GetPosition()));
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

void Rectangle::onPaint(wxPaintEvent &) {
    wxPaintDC device(this);
    wxGraphicsContext *gcd = wxGraphicsContext::Create(device);
    if(gcd) {
        wxPen outline(wxColour("red"), bestWidth / 3);
        gcd->SetPen(outline);
        gcd->SetBrush(wxBrush(wxColour(0, 0, 0, 0)));
        gcd->DrawRectangle(bestWidth, bestWidth, GetSize().GetWidth() - corner,
                GetSize().GetHeight() - corner);
        device.SetPen(outline);
        wxPoint north(GetSize().GetWidth() / 2, (GetSize().GetHeight() / 2) - (dragWidth / 2));
        wxPoint south(GetSize().GetWidth() / 2, (GetSize().GetHeight() / 2) + (dragWidth / 2));
        wxPoint east((GetSize().GetWidth() / 2) + (dragWidth / 2), GetSize().GetHeight() / 2);
        wxPoint west((GetSize().GetWidth() / 2) - (dragWidth / 2), GetSize().GetHeight() / 2);
        device.DrawLine(north, south);
        device.DrawLine(east, west);
        device.SetPen(wxPen("red", 1));
        device.SetBrush(wxBrush(wxColour("red")));
        device.DrawRectangle(nwz.GetX(), nwz.GetY(), nwz.GetWidth(), nwz.GetHeight());
        device.DrawRectangle(swz.GetX(), swz.GetY(), swz.GetWidth(), swz.GetHeight());
        device.DrawRectangle(nez.GetX(), nez.GetY(), nez.GetWidth(), nez.GetHeight());
        device.DrawRectangle(sez.GetX(), sez.GetY(), sez.GetWidth(), sez.GetHeight());
    }
}

Rectangle::~Rectangle() {

}
