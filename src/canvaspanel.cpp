#include "canvaspanel.h"
#include "defs.h"
#include "rectangle.h"
#include "cropevent.h"
#include <wx/graphics.h>

CanvasPanel::CanvasPanel(wxWindow *parent, wxWindowID id, wxBitmap *bm) :
        wxScrolledCanvas() {

    Create(parent, id);
    createElements(bm);
    initElements();
    setBindings();
}

void CanvasPanel::setBindings() {
    //Bind(wxEVT_PAINT, &CanvasPanel::onPaint, this);
    img->Bind(wxEVT_SIZE, &CanvasPanel::updateCropPosition, this);
    Bind(EVT_RECTANGLE_CHANGE, &CanvasPanel::sendCropChange, this);
    Bind(wxEVT_SCROLLWIN_THUMBTRACK, &CanvasPanel::saveCropPosition, this);
}

void CanvasPanel::saveCropPosition(wxScrollWinEvent &event) {
    HandleOnScroll(event);
    oldCropPosition = cropArea->GetPosition();
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
    cropArea->Move(img->GetPosition().x + cropOffset.x, img->GetPosition().y + cropOffset.y);
    oldCropPosition = cropArea->GetPosition();
    event.Skip();
}

void CanvasPanel::createElements(wxBitmap *bm) {
    if(bm) {
        sz = new wxGridSizer(1, 0, 0);
        img = new ImageWindow(this, wxID_ANY, bm);
        cropArea = new Rectangle(this, ict::CROP_AREA);
    } else {
        img = nullptr;
        cropArea = nullptr;
        sz = nullptr;
    }
}

void CanvasPanel::initElements() {
    if(img) {
        SetMinSize(wxSize(100, 100));
        ppuX = 2; ppuY = 2;
        SetScrollRate(ppuX, ppuY);
        SetVirtualSize(150, 150);
        sz->Add(img, 0, wxALIGN_CENTER | wxALL, 100);
        SetSizer(sz);
        oldCropPosition = img->GetPosition();
        cropOffset = wxPoint(0, 0);
        cropArea->SetSize(oldCropPosition.x, oldCropPosition.y, img->GetSize().GetWidth(), img->GetSize().GetHeight());
    }
}

void CanvasPanel::paintShadow(const wxRect &frame, wxGraphicsContext *gc) {
    if(!gc) return;
    wxColour bc(0, 0, 0, 80);
    wxColour tc(0, 0, 0, 0);
    gc->SetPen(wxPen(bc, 2));
    gc->SetBrush(wxBrush(tc));
    wxGraphicsPath shadow = gc->CreatePath();
    shadow.AddRoundedRectangle(frame.GetX() - 2, frame.GetY() - 2, 
                frame.GetWidth() + 4, frame.GetHeight() + 4, 2);
    gc->DrawPath(shadow);
}

void CanvasPanel::onPaint(wxPaintEvent &event) {
    wxPaintDC dev(this);
    DoPrepareDC(dev);
    wxGraphicsContext *gcd = wxGraphicsContext::Create(dev);
    if(img && gcd) {
        wxRect imgShadow(img->GetPosition().x, img->GetPosition().y, img->GetSize().GetWidth(), img->GetSize().GetHeight());
        paintShadow(imgShadow, gcd);
        delete gcd;
    }
}

CanvasPanel::~CanvasPanel() {

}
