#include "previewpanel.h"
#include "imgtools.h"

PreviewPanel::PreviewPanel(wxWindow *parent, wxWindowID id) : 
    wxScrolledCanvas(parent, id) {

    preview = nullptr;
    frame = new wxCustomBackgroundWindow<wxWindow>();
    frame->Create(this, wxID_ANY);
    sz = new wxBoxSizer(wxVERTICAL);
    sz->Add(frame);
    SetSizer(sz);
    SetScrollRate(5, 5);
}

void PreviewPanel::updatePreview(wxBitmap *bm) {
    if(!bm) return;
    preview = bm;
    frame->SetMinSize(preview->GetSize());
    frame->SetBackgroundBitmap(*preview);
}

PreviewPanel::~PreviewPanel() {
    if(frame) frame->Destroy();
}
