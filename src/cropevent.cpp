#include "cropevent.h"

wxDEFINE_EVENT(EVT_CROP_CHANGE, CropEvent);

CropEvent::
CropEvent(wxEventType eventType, int winId, const wxSize &s, wxPoint &o) :
    wxEvent(winId, eventType), size(s), offset(o) {

}

wxSize CropEvent::getSize() const {
    return size;
}

wxPoint CropEvent::getOffset() const {
    return offset;
}

wxEvent * CropEvent::Clone() const {
    return new CropEvent(*this);
}
