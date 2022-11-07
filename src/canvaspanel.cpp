#include "canvaspanel.h"
#include "defs.h"

CanvasPanel::CanvasPanel(wxWindow *parent, wxWindowID id) : 
        wxScrolledWindow() {
    Create(parent, id);

    test = new wxButton(this, ict::CROP_RECT, "TEST");
    sz = new wxBoxSizer(wxVERTICAL);
    cropArea = new Rectangle(this, wxID_ANY);
    cropArea->SetMinSize(wxSize(200, 200));
    sz->Add(test);
    sz->Add(cropArea);
    SetSizerAndFit(sz);
}
