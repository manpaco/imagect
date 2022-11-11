#ifndef CROPEVENT_H
#define CROPEVENT_H

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
        wxDECLARE_DYNAMIC_CLASS(CropEvent);

    private:
        wxSize size;

};

wxDECLARE_EVENT(EVT_CROP_CHANGE, CropEvent);

#endif // RECTANGLEEVENT_H
