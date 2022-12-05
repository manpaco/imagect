#include "previewpanel.h"
#include "imgtools.h"

PreviewPanel::PreviewPanel(wxWindow *parent, wxWindowID id) {
    Create(parent, id);
    initDimensions();
}

PreviewPanel::PreviewPanel(wxWindow *parent, wxWindowID id, wxBitmap &bm): PreviewPanel(parent, id) {
    updateElements(bm);
}

void PreviewPanel::clear() {
    if(preview) {
        sz->Detach(preview);
        attachedImg = false;
        preview->Destroy();
        preview = nullptr;
    }
}

void PreviewPanel::initDimensions() {
    SetMinSize(wxSize(150, 150));
}

void PreviewPanel::updateElements(wxBitmap &bm) {
    if(!bm.IsOk()) return;
    if(!preview) {
        preview = new ImageWindow(this, wxID_ANY, bm);
    } else preview->updateImage(bm);
    if(!sz) {
        sz = new wxBoxSizer(wxVERTICAL);
        title = new wxStaticText(this, wxID_ANY, "Preview");
        sz->Add(title, 0, wxALIGN_CENTER_HORIZONTAL);
        SetSizerAndFit(sz);
    }
    tryToAttachImg();
    Layout();
}

void PreviewPanel::tryToAttachImg() {
    if(attachedImg) return;
    sz->Add(preview, 0, wxALIGN_CENTER);
    attachedImg = true;
}

void PreviewPanel::updatePreview(wxBitmap &bm) {
    updateElements(bm);
}

PreviewPanel::~PreviewPanel() {

}
