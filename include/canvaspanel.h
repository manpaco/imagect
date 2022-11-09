#ifndef CANVASPANEL_H
#define CANVASPANEL_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/custombgwin.h"
#include "rectangle.h"

const int canvasOffset = 100;

class CanvasPanel: public wxScrolledWindow {
    public:
        CanvasPanel(wxWindow *parent, wxWindowID id, const wxBitmap &);

    private:
        void createObjects(const wxBitmap &bm);
        void onPaint(wxPaintEvent &event);

        Rectangle *cropArea;
        wxBitmap *img;

};

#endif // CANVASPANEL_H
