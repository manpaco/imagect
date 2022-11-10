#ifndef RECTANGLEEVENT_H
#define RECTANGLEEVENT_H
#include <wx/event.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class RectangleEvent : public wxEvent {
    public:
        RectangleEvent(wxEventType, int, const wxSize &);
        wxSize getSize() const;
        wxPoint getRelativePosition() const;

        virtual wxEvent * Clone() const;

    private:
        wxSize size;

};

wxDECLARE_EVENT(EVT_RECTANGLE_MOVE, RectangleEvent);
wxDECLARE_EVENT(EVT_RECTANGLE_RESIZE, RectangleEvent);

#endif // RECTANGLEEVENT_H
