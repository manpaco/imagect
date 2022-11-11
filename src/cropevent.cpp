#include "cropevent.h"

wxDEFINE_EVENT(EVT_CROP_MOVE, CropEvent);
wxDEFINE_EVENT(EVT_CROP_RESIZE, CropEvent);

CropEvent::CropEvent(wxEventType eventType, int winId, const wxSize &s) : wxEvent(eventType, winId), size(s) {

}

wxSize CropEvent::getSize() const {
    return size;
}

wxPoint CropEvent::getPosition() const {
    return wxPoint(size.GetX(), size.GetY());
}

wxEvent * CropEvent::Clone() const {
    return new CropEvent(*this);
}
