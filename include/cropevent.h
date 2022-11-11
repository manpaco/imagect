#ifndef CROPEVENT_H
#define CROPEVENT_H

#include <wx/event.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class CropEvent : public wxEvent {
    public:
        CropEvent(wxEventType, int, const wxSize &);
        wxSize getSize() const;
        wxPoint getPosition() const;

        virtual wxEvent * Clone() const;

    private:
        wxSize size;

};

wxDECLARE_EVENT(EVT_CROP_MOVE, CropEvent);
wxDECLARE_EVENT(EVT_CROP_RESIZE, CropEvent);

#endif // RECTANGLEEVENT_H
