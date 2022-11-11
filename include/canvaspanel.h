#ifndef CANVASPANEL_H
#define CANVASPANEL_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/custombgwin.h"
#include "rectangle.h"

const int virtualOffset = 100;

class CanvasPanel: public wxScrolledCanvas {
    public:
        CanvasPanel(wxWindow *parent, wxWindowID id, const wxBitmap &);
        wxPoint relativeCoords(const wxPoint &) const;
        wxPoint absoluteCoords(const wxPoint &) const;

    private:
        void createObjects(const wxBitmap &bm);
        void initObjects();
        void onPaint(wxPaintEvent &event);
        void updatePositions(wxSizeEvent &event);
        void updateScrollValues(wxScrollWinEvent &);
        void reportCropChange(wxCommandEvent &);
        void paintSquareShadow(const wxRect &, wxGraphicsContext *);

        Rectangle *cropArea;
        wxBitmap *img;

        wxPoint imgPosition;
        wxPoint scrollPosition;
        wxSize virtualSize;
        wxSize oldSize;
        bool glitchX, glitchY;
        int ppuX, ppuY;

};

#endif // CANVASPANEL_H
