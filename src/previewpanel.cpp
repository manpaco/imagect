#include "previewpanel.h"
#include "pixeltool.h"

PreviewPanel::PreviewPanel(wxWindow *parent, wxWindowID id) : 
    wxScrolledWindow(parent, id) {

    bm = nullptr;
    box = new wxCustomBackgroundWindow<wxPanel>();
    box->Create(this);
    sz = new wxBoxSizer(wxVERTICAL);
    sz->Add(box);
    SetSizer(sz);
    SetScrollRate(5, 5);
}

void PreviewPanel::createBitmap(Magick::Image &img) {
    if(bm) delete(bm);
    PixelTool extractor;
    bm = new wxBitmap(wxImage(img.columns(), img.rows(), 
            extractor.extractRgb(img), extractor.extractAlpha(img)));
}

void PreviewPanel::updatePreview(Magick::Image &img) {
    createBitmap(img);
    box->SetMinSize(wxSize(img.columns(), img.rows()));
    box->SetBackgroundBitmap(*bm);
}
