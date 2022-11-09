#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "defs.h"

const int borderWidth = 4;
const int corner = borderWidth * 2;
const int resizeLimit = corner * 3;


class Rectangle : public wxControl {
    public:
        Rectangle();
        Rectangle(wxWindow *parent, wxWindowID id, 
                const wxPoint &pos=wxDefaultPosition, 
                const wxSize &size=wxDefaultSize, long style=wxBORDER_NONE, 
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
        void updateSizes(wxSizeEvent &);

    private:
        ict::Zone getLocation(const wxPoint);
        bool isContained(wxRect area, wxPoint point);
        void changeCursor(ict::Zone type);
        void resizeUsing(ict::Zone);

        wxDECLARE_DYNAMIC_CLASS(Rectangle);
        wxPoint clientPressPoint;
        wxRect iz, nz, sz, ez, wz, nez, nwz, sez, swz;

        ict::Zone zonePressed = ict::NONE;
        long ratio;
};

#endif // RECTANGLE_H
