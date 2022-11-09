#include "canvaspanel.h"
#include "defs.h"

CanvasPanel::CanvasPanel(wxWindow *parent, wxWindowID id, const wxBitmap &bm) :
        wxScrolledWindow() {

    Create(parent, id);
    createObjects(bm);
    SetScrollRate(5, 5);
    Bind(wxEVT_PAINT, &CanvasPanel::onPaint, this);
}

void CanvasPanel::createObjects(const wxBitmap &bm) {
    if(bm.IsOk()) {
        img = new wxBitmap(bm);
        cropArea = new Rectangle(this, ict::CROP_AREA);
        cropArea->SetSize(canvasOffset, canvasOffset, img->GetWidth(), img->GetHeight());
        SetVirtualSize((canvasOffset * 2) + img->GetWidth(), (canvasOffset * 2) + img->GetHeight());
    } else {
        img = nullptr;
        cropArea = nullptr;
    }
}

void CanvasPanel::onPaint(wxPaintEvent &event) {
    wxPaintDC dev(this);
    DoPrepareDC(dev);
    if(img) dev.DrawBitmap(*img, wxPoint(canvasOffset, canvasOffset));
}
