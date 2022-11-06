#include "previewpanel.h"
#include "pixeltool.h"

PreviewPanel::PreviewPanel(wxWindow *parent, wxWindowID id) : 
    wxScrolledWindow(parent, id) {
    Bind(wxEVT_PAINT, &PreviewPanel::paintImage, this);
}

void PreviewPanel::paintImage(wxPaintEvent &event) {
    wxPaintDC previewDC(this);
    previewDC.DrawBitmap(*bm, 0, 0);
}

void PreviewPanel::createBitmap(Magick::Image &img) {
    if(bm) delete(bm);
    PixelTool extractor;
    bm = new wxBitmap(wxImage(img.columns(), img.rows(), 
            extractor.extractRgb(img), extractor.extractAlpha(img), true));
}

void PreviewPanel::updatePreview(Magick::Image &img) {
    createBitmap(img);
    Update();
}
