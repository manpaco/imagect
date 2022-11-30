#include "canvaspanel.h"
#include "rectangle.h"
#include "cropevent.h"
#include <wx/graphics.h>

CanvasPanel::CanvasPanel(wxWindow *parent, wxWindowID id, wxBitmap &bm) {
    Create(parent, id);
    createElements(bm);
    initParams();
    setBindings();
}

CanvasPanel::CanvasPanel(wxWindow *parent, wxWindowID id) {
    Create(parent, id);
    initParams();
    setBindings();
}

void CanvasPanel::setBindings() {
    img->Bind(wxEVT_SIZE, &CanvasPanel::updateCropPosition, this);
    Bind(EVT_RECTANGLE_CHANGE, &CanvasPanel::sendCropChange, this);
    Bind(wxEVT_SCROLLWIN_THUMBTRACK, &CanvasPanel::saveCropPosition, this);
}

void CanvasPanel::saveCropPosition(wxScrollWinEvent &event) {
    HandleOnScroll(event);
    oldCropPosition = cropArea->GetPosition();
    wxRect r(img->GetPosition().x, img->GetPosition().y, img->GetSize().GetWidth(), img->GetSize().GetHeight());
    cropArea->setRestrictions(r);
    event.Skip();
}

void CanvasPanel::sendCropChange(wxCommandEvent &event) {
    wxSize size(cropArea->GetSize());
    cropOffset += cropArea->GetPosition() - oldCropPosition;
    oldCropPosition = cropArea->GetPosition();
    CropEvent toSend(EVT_CROP_CHANGE, GetId(), size, cropOffset);
    toSend.SetEventObject(this);
    ProcessWindowEvent(toSend);
}

void CanvasPanel::updateCropPosition(wxSizeEvent &event) {
    shadow->Move(img->GetPosition().x - 2, img->GetPosition().y - 2);
    cropArea->Move(img->GetPosition().x + cropOffset.x, img->GetPosition().y + cropOffset.y);
    oldCropPosition = cropArea->GetPosition();
    wxRect r(img->GetPosition().x, img->GetPosition().y, img->GetSize().GetWidth(), img->GetSize().GetHeight());
    cropArea->setRestrictions(r);
    event.Skip();
}

void CanvasPanel::updateCanvas(wxBitmap &bm) {
    createElements(bm);
}

void CanvasPanel::createElements(wxBitmap &bm) {
    if(!bm.IsOk()) return;
    if(!shadow) shadow = new wxWindow(this, wxID_ANY);
    if(!img) img = new ImageWindow(this, wxID_ANY, bm);
    else img->updateImage(bm);
    initShadow();
    if(!sz) {
        sz = new wxGridSizer(1, 0, 0);
        initSizer();
    }
    if(!cropArea) cropArea = new Rectangle(this, ict::CROP_AREA);
    initCrop();
}

void CanvasPanel::initSizer() {
    sz->Add(img, 0, wxALIGN_CENTER | wxALL, 100);
    SetSizer(sz);
}

void CanvasPanel::initShadow() {
    wxColour bc(0, 0, 0, 80);
    wxSize shsz(img->GetSize());
    shsz += wxSize(4, 4);
    shadow->SetSize(shsz);
    shadow->SetMinSize(shsz);
    shadow->SetBackgroundColour(bc);
}

void CanvasPanel::initCrop() {
    oldCropPosition = img->GetPosition();
    cropOffset = wxPoint(0, 0);
    cropArea->SetSize(oldCropPosition.x, oldCropPosition.y, img->GetSize().GetWidth(), img->GetSize().GetHeight());
    wxRect r(oldCropPosition.x, oldCropPosition.y, img->GetSize().GetWidth(), img->GetSize().GetHeight());
    cropArea->setRestrictions(r);
    cropArea->activateRestrictions(true);
}

void CanvasPanel::initParams() {
    SetMinSize(wxSize(100, 100));
    ppuX = 2; ppuY = 2;
    SetScrollRate(ppuX, ppuY);
    SetVirtualSize(150, 150);
}

CanvasPanel::~CanvasPanel() {

}
