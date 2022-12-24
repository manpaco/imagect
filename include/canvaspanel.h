#ifndef CANVASPANEL_H
#define CANVASPANEL_H

class DuctileRectangle;
class ImageWindow;
class wxGridSizer;
class wxGraphicsContext;

#include "wx/scrolwin.h"

class CanvasPanel: public wxScrolledCanvas {
    public:
        CanvasPanel(wxWindow *parent, wxWindowID id, wxBitmap &);
        CanvasPanel(wxWindow *parent, wxWindowID id);
        void updateCanvas(wxBitmap &);
        void cropSize(const wxSize &s);
        wxSize cropSize();
        wxPoint cropOffset() const;
        void cropGeometry(const wxRect &g);
        void fixCrop(bool);
        void allowGrow(bool);
        void clear();
        ~CanvasPanel();

    private:
        void updateElements(wxBitmap &bm);
        void initElements();
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

        DuctileRectangle *cropArea = nullptr;
        ImageWindow *img = nullptr;
        wxGridSizer *sz = nullptr;
        wxWindow *shadow = nullptr;

        wxPoint oldCropPosition;
        wxPoint cropOff;
        int ppuX, ppuY;
        bool attachedImg = false;

        const int virtualOffset = 100;
};

#endif // CANVASPANEL_H
