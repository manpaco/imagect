#ifndef CANVASPANEL_H
#define CANVASPANEL_H

class wxGraphicsContext;
namespace Magick {
    class Image;
}

#include "wx/panel.h"
#include "controller.h"
#include "defs.h"

class CanvasPanel: public wxPanel {
    public:
        CanvasPanel(wxWindow *parent, wxWindowID id, Magick::Image *);
        CanvasPanel(wxWindow *parent, wxWindowID id);
        bool cropSize(wxSize *s);
        bool cropGeometry(wxRect *g);
        wxRect cropGeometry() const;
        wxSize cropSize() const;
        wxPoint cropOffset() const;
        void fixCrop(bool);
        bool allowGrow(bool);
        void setScaleFactor(float sf);
        wxRect translateRect(const wxRect &r, ict::Tot, ict::Dot) const;
        wxPoint translatePoint(const wxPoint &p, ict::Tot, ict::Dot) const;
        wxSize translateSize(const wxSize &s, ict::Tot, ict::Dot) const;
        ~CanvasPanel();

    private:
        void onPaint(wxPaintEvent &event);
        void sendCropEvent();
        void changeCursor(ict::Zone type);
        int translate(int v, ict::Tot t, ict::Dot d) const;
        void paintCropRect(const wxRect &paint, wxGraphicsContext *gc);
        void initBuffer(wxBitmap &);
        void initCanvas(wxBitmap &);
        void updateSizes();
        void refreshCanvas();
        void refreshDamaged();
        wxRect shadowRect();
        void mouseMotion(wxMouseEvent &event);
        void mousePress(wxMouseEvent &event);
        void mouseRelease(wxMouseEvent &event);
        wxPoint relativeToImage(const wxPoint &ap) const;
        wxPoint absoluteCoords(const wxPoint &rp) const;
        void compressImage(Magick::Image *);

        CropController controller;
        wxBitmap *buffer = nullptr;
        wxRect imgRect;
        wxSize bufferSize;
        float scaleFactor = 1.0, compressFactor = 1.0;

        wxPoint lastPoint;
        wxRect prevCrop;
};

#endif // CANVASPANEL_H
