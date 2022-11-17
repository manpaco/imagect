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
        CanvasPanel(wxWindow *parent, wxWindowID id, wxBitmap &);
        CanvasPanel(wxWindow *parent, wxWindowID id);
        void updateCanvas(wxBitmap &);
        ~CanvasPanel();

    private:
        void createElements(wxBitmap &bm);
        void initElements();
        void updateCropPosition(wxSizeEvent &event);
        void sendCropChange(wxCommandEvent &);
        void paintShadow(const wxRect &, wxGraphicsContext *);
        void setBindings(); 
        void saveCropPosition(wxScrollWinEvent &event);
        void initSizer();
        void initShadow();
        void initCrop();
        void initParams();

        Rectangle *cropArea = nullptr;
        ImageWindow *img = nullptr;
        wxGridSizer *sz = nullptr;
        wxWindow *shadow = nullptr;

        wxPoint oldCropPosition;
        wxPoint cropOffset;
        int ppuX, ppuY;
};

#endif // CANVASPANEL_H
