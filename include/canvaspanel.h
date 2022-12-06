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
        void cropSize(wxSize &s);
        wxSize cropSize();
        wxPoint getCropOffset() const;
        void cropGeometry(const wxRect &g);
        void fixCrop(bool);
        void allowGrow(bool);
        void clear();
        ~CanvasPanel();

    private:
        void updateElements(wxBitmap &bm);
        void initElements();
        void reportCollateral(wxCommandEvent &);
        void updateCropPosition(wxSizeEvent &event);
        void reportChange(wxCommandEvent &);
        void sendCropEvent();
        void paintShadow(const wxRect &, wxGraphicsContext *);
        void bindCrop(); 
        void unbindCrop();
        void saveCropPosition(wxScrollWinEvent &event);
        void createSizer();
        void initShadow();
        void initCrop();
        void initParams();
        void tryToAttachImg();

        Rectangle *cropArea = nullptr;
        ImageWindow *img = nullptr;
        wxGridSizer *sz = nullptr;
        wxWindow *shadow = nullptr;

        wxPoint oldCropPosition;
        wxPoint cropOffset;
        int ppuX, ppuY;
        bool attachedImg = false;
};

#endif // CANVASPANEL_H
