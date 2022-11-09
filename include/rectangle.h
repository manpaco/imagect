#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "defs.h"

class Rectangle : public wxControl {
    public:
        Rectangle();
        Rectangle(wxWindow *parent, wxWindowID id, 
                const wxPoint &pos=wxDefaultPosition, 
                const wxSize &size=wxDefaultSize, long style=0, 
                const wxValidator &validator=wxDefaultValidator, 
                const wxString &name=wxControlNameStr);    
        ~Rectangle();

    protected:
        void Init();
        void OnPaint(wxPaintEvent &);
        void mouseMotion(wxMouseEvent &);
        void mousePress(wxMouseEvent &);
        void mouseRelease(wxMouseEvent &);
        void resetCursor(wxMouseEvent &);

    private:
        bool innerZone(const wxPoint);
        ict::CardinalPoint edgeZone(const wxPoint);
        bool isContained(wxRect area, wxPoint point);
        void changeCursor(ict::CardinalPoint type);

        wxDECLARE_DYNAMIC_CLASS(RectSash);
        wxSize area;
        bool drag = false;
        ict::CardinalPoint resize;
};

#endif // RECTANGLE_H
