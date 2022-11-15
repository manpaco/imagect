#include "imagewindow.h"

ImageWindow::ImageWindow(wxWindow *parent, wxWindowID id, wxBitmap *bm, const wxPoint &pos, const wxSize &size) {
    Create(parent, id, pos, size);
    this->bm = bm;
    SetMinSize(wxSize(bm->GetSize().GetWidth(), bm->GetSize().GetHeight()));
    SetSize(wxSize(bm->GetSize().GetWidth(), bm->GetSize().GetHeight()));
    Bind(wxEVT_PAINT, &ImageWindow::onPaint, this);
}

void ImageWindow::onPaint(wxPaintEvent &event) {
    if(bm) {
        wxPaintDC dev(this);
        dev.DrawBitmap(*bm, 0, 0);
    }
}

ImageWindow::~ImageWindow() {

}
