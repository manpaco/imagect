#include "zoomevent.h"

wxDEFINE_EVENT(EVT_ZOOM_CHANGE, ZoomEvent);

ZoomEvent::ZoomEvent(wxEventType eventType, int winId, float sf) : wxEvent(winId, eventType), scaleFactor(sf) {

}

float ZoomEvent::getScaleFactor() const {
    return scaleFactor;
}

wxEvent * ZoomEvent::Clone() const {
    return new ZoomEvent(*this);
}
