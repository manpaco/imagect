#include "rectangleevent.h"

wxDEFINE_EVENT(EVT_RECTANGLE_MOVE, RectangleEvent);
wxDEFINE_EVENT(EVT_RECTANGLE_RESIZE, RectangleEvent);

RectangleEvent::RectangleEvent(wxEventType eventType, int winId, const wxSize &s) : wxEvent(eventType, winId), size(s) {

}

wxSize RectangleEvent::getSize() const {
    return size;
}

wxPoint RectangleEvent::getRelativePosition() const {
    return wxPoint(size.GetX(), size.GetY());
}

wxEvent * RectangleEvent::Clone() const {
    return new RectangleEvent(*this);
}
