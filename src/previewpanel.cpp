#include "previewpanel.h"
#include "imgtools.h"

extern const int bestSpace;

PreviewPanel::PreviewPanel(wxWindow *parent, wxWindowID id) {
    Create(parent, id);
    createSizer();
    initParams();
}

PreviewPanel::PreviewPanel(wxWindow *parent, wxWindowID id, wxBitmap &bm): PreviewPanel(parent, id) {
    updateElements(bm);
}

void PreviewPanel::createSizer() {
    sz = new wxBoxSizer(wxVERTICAL);
    title = new wxStaticText(this, wxID_ANY, "Preview");
    sz->Add(title, 0, wxALIGN_CENTER);
    sz->AddSpacer(bestSpace);
    sz->Add(new wxStaticLine(this), 0, wxEXPAND);
    imgSz = new wxGridSizer(1, 0, 0);
    sz->Add(imgSz, 1, wxEXPAND);
    SetSizer(sz);
}

void PreviewPanel::clear() {
    if(preview) {
        imgSz->Detach(preview);
        attachedImg = false;
        preview->Destroy();
        preview = nullptr;
    }
}

void PreviewPanel::initParams() {
    SetMinSize(wxSize(150, 150));
}

void PreviewPanel::updateElements(wxBitmap &bm) {
    if(!bm.IsOk()) return;
    if(!preview) {
        preview = new ImageWindow(this, wxID_ANY, bm);
    } else preview->updateImage(bm);
    tryToAttachImg();
    Layout();
}

void PreviewPanel::tryToAttachImg() {
    if(attachedImg) return;
    imgSz->Add(preview, 0, wxALIGN_CENTER);
    attachedImg = true;
}

void PreviewPanel::updatePreview(wxBitmap &bm) {
    updateElements(bm);
}

PreviewPanel::~PreviewPanel() {

}
