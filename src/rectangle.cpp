#include "rectangle.h"
#include "exceptionsdef.h"
#include <iterator>
#include <wx/graphics.h>
#include <wx/gtk/colour.h>

wxIMPLEMENT_DYNAMIC_CLASS(Rectangle, wxControl);
wxDEFINE_EVENT(EVT_RECTANGLE_CHANGE, wxCommandEvent);

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
    iz = wxRect(0, 0, event.GetSize().GetWidth(), event.GetSize().GetHeight());
    nz = wxRect(0, 0, event.GetSize().GetWidth(), corner);
    sz = wxRect(0, event.GetSize().GetHeight() - corner, event.GetSize().GetWidth(), corner);
    ez = wxRect(event.GetSize().GetWidth() - corner, 0, corner, event.GetSize().GetHeight());
    wz = wxRect(0, 0, corner, event.GetSize().GetHeight());
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
    if(!zonePressed) {
        changeCursor(getLocation(event.GetPosition()));
        event.Skip();
        return;
    }
    if(zonePressed == ict::INNER) {
        wxPoint screenPoint(wxGetMousePosition());
        screenPoint = screenPoint - clientPressPoint;
        Move(GetParent()->ScreenToClient(screenPoint));
    } else {
        resizeUsing(zonePressed);
    }
    changed = true;
    event.Skip();
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
        deltaX = bestWidth;
        deltaY = bestWidth;
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
        deltaX = -bestWidth;
        deltaY = -bestWidth;
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
        deltaX = bestWidth;
        deltaY = -bestWidth;
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
        deltaX = -bestWidth;
        deltaY = bestWidth;
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
        deltaY = -bestWidth;
        deltaY += mousePosition.y - positionOnScreen.y;
        if(mousePosition.y > limitPosY) return;
        SetSize(wxDefaultCoord, positionOnParent.y + deltaY, wxDefaultCoord, 
                GetSize().GetHeight() - deltaY, wxSIZE_USE_EXISTING);
        return;
    }
    if(zone == ict::S) {
        limitPosY = positionOnScreen.y + resizeLimit;
        deltaY = bestWidth;
        deltaY += mousePosition.y - 
                (positionOnScreen.y + GetSize().GetHeight());
        if(mousePosition.y < limitPosY) return;
        SetSize(wxDefaultCoord, wxDefaultCoord, wxDefaultCoord, 
                GetSize().GetHeight() + deltaY, wxSIZE_USE_EXISTING);
        return;
    }
    if(zone == ict::W) {
        limitPosX = (positionOnScreen.x + GetSize().GetWidth()) - resizeLimit;
        deltaX = -bestWidth;
        deltaX += mousePosition.x - positionOnScreen.x;
        if(mousePosition.x > limitPosX) return;
        SetSize(positionOnParent.x + deltaX, wxDefaultCoord, 
                GetSize().GetWidth() - deltaX, wxDefaultCoord, 
                wxSIZE_USE_EXISTING);
        return;
    }
    if(zone == ict::E) {
        limitPosX = positionOnScreen.x + resizeLimit;
        deltaX = bestWidth;
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
    try {
        if(!HasCapture()) CaptureMouse();
        else throw std::runtime_error("Error code: ");
    } catch (std::runtime_error &e) {
        std::cout << e.what() << MOUSE_ALREDY_CAPTURED << std::endl;
    }
    clientPressPoint = event.GetPosition();
    zonePressed = getLocation(event.GetPosition());
}

void Rectangle::mouseRelease(wxMouseEvent &event) {
    try {
        if(HasCapture()) ReleaseMouse();
        else throw std::runtime_error("Error code: ");
    } catch (std::runtime_error &e) {
        std::cout << e.what() << MOUSE_NEVER_CAPTURED << std::endl;
    }
    if(changed) {
        wxCommandEvent toSend(EVT_RECTANGLE_CHANGE, GetId());
        toSend.SetEventObject(this);
        ProcessWindowEvent(toSend);
        changed = false;
    }
    zonePressed = ict::NONE;
    if(mouseLeftWin) changeCursor(ict::NONE);
    else changeCursor(getLocation(event.GetPosition()));
}

ict::Zone Rectangle::getLocation(const wxPoint p) {
    if(isContained(nez, p)) return ict::NE;
    if(isContained(nwz, p)) return ict::NW;
    if(isContained(sez, p)) return ict::SE;
    if(isContained(swz, p)) return ict::SW;
    if(isContained(nz, p)) return ict::N;
    if(isContained(sz, p)) return ict::S;
    if(isContained(ez, p)) return ict::E;
    if(isContained(wz, p)) return ict::W;
    if(isContained(iz, p)) return ict::INNER;
    return ict::NONE;
}

bool Rectangle::isContained(wxRect area, wxPoint point) {
    int rightSide = area.GetWidth() + area.GetX();
    int bottomSide = area.GetHeight() + area.GetY();
    if((area.GetX() <= point.x && point.x <= rightSide) &&
        (area.GetY() <= point.y && point.y <= bottomSide)) return true;
    return false;
}

void Rectangle::paintSpecialFrame(const wxRect &paint, wxGraphicsContext *gc, bool fill) {
    wxPen wLine(wxColour(*wxWHITE), 1);
    wxPen bLine(wxColour(*wxBLACK), 1);
    gc->SetBrush(wxBrush(wxColour(0, 0, 0, 0)));
    gc->SetPen(bLine);
    gc->DrawRectangle(paint.GetX(), paint.GetY(), paint.GetWidth() - 1, paint.GetHeight() - 1);
    gc->SetPen(wLine);
    gc->DrawRectangle(paint.GetX() + 1, paint.GetY() + 1, paint.GetWidth() - 3, paint.GetHeight() - 3);
    gc->SetPen(bLine);
    if(fill) gc->SetBrush(wxBrush(wxColour(*wxWHITE)));
    gc->DrawRectangle(paint.GetX() + 2, paint.GetY() + 2, paint.GetWidth() - 5, paint.GetHeight() - 5);
}

void Rectangle::onPaint(wxPaintEvent &) {
    wxPaintDC device(this);
    wxGraphicsContext *gcd = wxGraphicsContext::Create(device);
    if(gcd) {
        paintSpecialFrame(wxRect(0, 0, GetSize().GetWidth(), GetSize().GetHeight()), gcd, false);
        paintSpecialFrame(nwz, gcd, true);
        paintSpecialFrame(nez, gcd, true);
        paintSpecialFrame(sez, gcd, true);
        paintSpecialFrame(swz, gcd, true);
        delete gcd;
    }
}

Rectangle::~Rectangle() {

}
