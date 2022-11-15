#ifndef CANVASPANEL_H
#define CANVASPANEL_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/custombgwin.h"
#include "rectangle.h"
#include "imagewindow.h"

const int virtualOffset = 100;

class CanvasPanel: public wxScrolledCanvas {
    public:
        CanvasPanel(wxWindow *parent, wxWindowID id, wxBitmap *);
        wxPoint relativeCoords(const wxPoint &) const;
        wxPoint absoluteCoords(const wxPoint &) const;
        ~CanvasPanel();

    private:
        void createElements(wxBitmap *bm);
        void initElements();
        void onPaint(wxPaintEvent &event);
        void updateCropPosition(wxSizeEvent &event);
        void sendCropChange(wxCommandEvent &);
        void paintShadow(const wxRect &, wxGraphicsContext *);
        void setBindings(); 

        Rectangle *cropArea;
        ImageWindow *img;
        wxGridSizer *sz;

        wxPoint oldCropPosition;
        wxPoint cropOffset;
        int ppuX, ppuY;
};

#endif // CANVASPANEL_H
