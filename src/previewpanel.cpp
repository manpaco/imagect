#include "previewpanel.h"
#include "imgtools.h"

PreviewPanel::PreviewPanel(wxWindow *parent, wxWindowID id) {
    Create(parent, id);
    init();
    initDimensions();
}

PreviewPanel::PreviewPanel(wxWindow *parent, wxWindowID id, wxBitmap &bm) {
    Create(parent, id);
    init();
    initDimensions();
    setPreview(bm);
}

void PreviewPanel::init() {
    title = new wxStaticText(this, wxID_ANY, "Preview");
    sz = new wxBoxSizer(wxVERTICAL);
    sz->Add(title, 0, wxALIGN_CENTER_HORIZONTAL);
    SetSizerAndFit(sz);
}

void PreviewPanel::initDimensions() {
    SetMinSize(wxSize(150, 150));
}

void PreviewPanel::setPreview(wxBitmap &bm) {
    if(!bm.IsOk()) return;
    if(!preview) {
        preview = new ImageWindow(this, wxID_ANY, bm);
        sz->Add(preview, 0, wxALIGN_CENTER);
        Fit();
    } else preview->updateImage(bm);
    Layout();
}

void PreviewPanel::updatePreview(wxBitmap &bm) {
    setPreview(bm);
}

PreviewPanel::~PreviewPanel() {

}
