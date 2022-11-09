#include "canvaspanel.h"
#include "defs.h"

CanvasPanel::CanvasPanel(wxWindow *parent, wxWindowID id, const wxBitmap &bm) :
        wxScrolledWindow() {

    Create(parent, id);
    createImg(bm);
    sz = new wxBoxSizer(wxVERTICAL);
    cropArea = new Rectangle(this, ict::CROP_AREA);
    cropArea->SetSize(wxSize(200, 200));
    sz->Add(img);
    SetSizer(sz);
    SetScrollRate(5, 5);
    FitInside();
}

void CanvasPanel::createImg(const wxBitmap &bm) {
    img = new wxCustomBackgroundWindow<wxPanel>();
    img->Create(this, wxID_ANY);
    img->SetMinSize(wxSize(bm.GetWidth(), bm.GetHeight()));
    img->SetBackgroundBitmap(bm);
}
