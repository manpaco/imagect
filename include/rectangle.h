#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "defs.h"

const int bestWidth = 6;
const int corner = bestWidth * 2;
const int dragWidth = corner * 4;
const int resizeLimit = (corner * 2) + dragWidth;


class Rectangle : public wxControl {
    public:
        Rectangle();
        Rectangle(wxWindow *parent, wxWindowID id, 
                const wxPoint &pos=wxDefaultPosition, 
                const wxSize &size=wxDefaultSize, long style=wxBORDER_NONE, 
                const wxValidator &validator=wxDefaultValidator, 
                const wxString &name=wxControlNameStr);    
        ~Rectangle();

    private:
        ict::Zone getLocation(const wxPoint);
        bool isContained(wxRect area, wxPoint point);
        void changeCursor(ict::Zone type);
        void resizeUsing(ict::Zone);
        void init();
        void onPaint(wxPaintEvent &);
        void mouseMotion(wxMouseEvent &);
        void mousePress(wxMouseEvent &);
        void mouseRelease(wxMouseEvent &);
        void leaveWinHandler(wxMouseEvent &);
        void enterWinHandler(wxMouseEvent &);
        void updateSizes(wxSizeEvent &);

        wxDECLARE_DYNAMIC_CLASS(Rectangle);
        wxPoint clientPressPoint;
        wxRect iz, nz, sz, ez, wz, nez, nwz, sez, swz;
        bool mouseLeftWin = true;

        ict::Zone zonePressed = ict::NONE;
        float ratio;
};

#endif // RECTANGLE_H
