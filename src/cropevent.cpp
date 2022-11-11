#include "cropevent.h"

wxIMPLEMENT_ABSTRACT_CLASS(CropEvent, wxEvent);
wxDEFINE_EVENT(EVT_CROP_CHANGE, CropEvent);

CropEvent::CropEvent(wxEventType eventType, int winId, const wxSize &s) : wxEvent(winId, eventType), size(s) {

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
