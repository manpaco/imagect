#include "canvaspanel.h"
#include "rectangle.h"
#include "imagewindow.h"
#include "cropevent.h"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

CanvasPanel::CanvasPanel(wxWindow *parent, wxWindowID id) {
    Create(parent, id);
    createSizer();
    initParams();
}

CanvasPanel::CanvasPanel(wxWindow *parent, wxWindowID id, wxBitmap &bm): CanvasPanel(parent, id) {
    updateElements(bm);
}

void CanvasPanel::createSizer() {
    sz = new wxGridSizer(1, 0, 0);
    SetSizer(sz);
}

void CanvasPanel::clear() {
    if(cropArea) {
        unbindCrop();
        cropArea->Destroy();
        cropArea = nullptr;
    }
    if(img) {
        sz->Detach(img);
        attachedImg = false;
        img->Destroy();
        img = nullptr;
    }
    if(shadow) {
        shadow->Destroy();
        shadow = nullptr;
    }
}

void CanvasPanel::bindCrop() {
    img->Bind(wxEVT_SIZE, &CanvasPanel::updateCropPosition, this);
    Bind(EVT_RECTANGLE_CHANGE, &CanvasPanel::reportChange, this);
    Bind(wxEVT_SCROLLWIN_THUMBTRACK, &CanvasPanel::saveCropPosition, this);
}

void CanvasPanel::unbindCrop() {
    img->Unbind(wxEVT_SIZE, &CanvasPanel::updateCropPosition, this);
    Unbind(EVT_RECTANGLE_CHANGE, &CanvasPanel::reportChange, this);
    Unbind(wxEVT_SCROLLWIN_THUMBTRACK, &CanvasPanel::saveCropPosition, this);
}

void CanvasPanel::reportChange(wxCommandEvent &event) {
    sendCropEvent();
}

void CanvasPanel::saveCropPosition(wxScrollWinEvent &event) {
    HandleOnScroll(event);
    oldCropPosition = cropArea->GetPosition();
    cropArea->setRestrictions(img->GetRect());
    event.Skip();
}

void CanvasPanel::sendCropEvent() {
    wxSize size(cropArea->GetSize());
    cropOff += cropArea->GetPosition() - oldCropPosition;
    oldCropPosition = cropArea->GetPosition();
    CropEvent toSend(EVT_CROP_CHANGE, GetId(), size, cropOff);
    toSend.SetEventObject(this);
    ProcessWindowEvent(toSend);
}

void CanvasPanel::updateCropPosition(wxSizeEvent &event) {
    shadow->Move(img->GetPosition().x - 2, img->GetPosition().y - 2);
    cropArea->Move(img->GetPosition().x + cropOff.x, img->GetPosition().y + cropOff.y);
    oldCropPosition = cropArea->GetPosition();
    cropArea->setRestrictions(img->GetRect());
    event.Skip();
}

void CanvasPanel::cropGeometry(const wxRect &g) {
    wxRect newG(img->GetPosition().x + g.GetX(), img->GetPosition().y + g.GetY(), g.GetWidth(), g.GetHeight());
    if(g.GetSize().x < 0) newG.SetWidth(img->GetSize().x);
    if(g.GetSize().y < 0) newG.SetHeight(img->GetSize().y);
    cropArea->setGeometry(newG);
}

void CanvasPanel::fixCrop(bool op) {
    cropArea->fixRatio(op);
}

void CanvasPanel::allowGrow(bool op) {
    cropArea->activateRestrictions(!op);
}

wxPoint CanvasPanel::cropOffset() const {
    return cropOff;
}

void CanvasPanel::cropSize(const wxSize &s) {
    cropArea->setGeometry(wxRect(cropArea->GetPosition(),s));
}

wxSize CanvasPanel::cropSize() {
    return cropArea->GetSize();
}

void CanvasPanel::updateCanvas(wxBitmap &bm) {
    updateElements(bm);
}

void CanvasPanel::updateElements(wxBitmap &bm) {
    if(!bm.IsOk()) return;
    if(!shadow) shadow = new wxWindow(this, wxID_ANY);
    if(!img) {
        img = new ImageWindow(this, wxID_ANY, bm);
    } else img->updateImage(bm);
    initShadow();
    tryToAttachImg();
    if(!cropArea) {
        cropArea = new DuctileRectangle(this, ict::CROP_AREA);
        bindCrop();
    }
    initCrop();
    Layout();
    Refresh();
}

void CanvasPanel::tryToAttachImg() {
    if(attachedImg) return;
    sz->Add(img, 0, wxALIGN_CENTER | wxALL, 100);
    attachedImg = true;
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
    cropOff = wxPoint(0, 0);
    cropArea->SetSize(oldCropPosition.x, oldCropPosition.y, img->GetSize().GetWidth(), img->GetSize().GetHeight());
    cropArea->setRestrictions(img->GetRect());
}

void CanvasPanel::initParams() {
    SetMinSize(wxSize(100, 100));
    ppuX = 2; ppuY = 2;
    SetScrollRate(ppuX, ppuY);
    SetVirtualSize(150, 150);
}

CanvasPanel::~CanvasPanel() {

}
