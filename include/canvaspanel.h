#ifndef CANVASPANEL_H
#define CANVASPANEL_H

class wxGraphicsContext;

#include "wx/panel.h"
#include "controller.h"
#include "defs.h"

class CanvasPanel: public wxPanel {
    public:
        CanvasPanel(wxWindow *parent, wxWindowID id, wxBitmap &);
        CanvasPanel(wxWindow *parent, wxWindowID id);
        bool cropSize(wxSize *s);
        bool cropGeometry(wxRect *g);
        wxSize cropSize() const;
        wxPoint cropOffset() const;
        void fixCrop(bool);
        void allowGrow(bool);
        void setScaleFactor(float sf);
        ~CanvasPanel();

    private:
        void onPaint(wxPaintEvent &event);
        void sendCropEvent();
        int translateIn(int v) const;
        int translateOut(int v) const;
        void changeCursor(ict::Zone type);
        void updatePaintBuffer(bool force = false);
        wxRect translateRectIn(const wxRect &r) const;
        wxRect translateRectOut(const wxRect &r) const;
        wxPoint translatePointIn(const wxPoint &p) const;
        wxPoint translatePointOut(const wxPoint &p) const;
        wxSize translateSizeIn(const wxSize &s) const;
        wxSize translateSizeOut(const wxSize &s) const;
        wxBitmap createCropBitmap();
        void paintSpecialFrame(const wxRect &paint, wxGraphicsContext *gc, bool fill);
        void initBuffers();
        void initCanvas(wxBitmap &);
        void initSizes();
        void refreshCanvas();
        wxRect shadowRect();
        void mouseMotion(wxMouseEvent &event);
        void mousePress(wxMouseEvent &event);
        void mouseRelease(wxMouseEvent &event);
        wxPoint relativeToImage(const wxPoint &ap, bool scaled = false) const;
        wxPoint absoluteCoords(const wxPoint &rp, bool scaled = false) const;

        CropController controller;
        wxBitmap *baseBuffer = nullptr;
        wxBitmap *paintBuffer = nullptr;
        wxBitmap *img = nullptr;
        wxRect srcImgRect;
        float scaleFactor = 1.0;

        wxPoint lastPoint;
        wxRect prevCrop;
};

#endif // CANVASPANEL_H
