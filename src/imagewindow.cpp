#include "imagewindow.h"

ImageWindow::ImageWindow(wxWindow *parent, wxWindowID id, wxBitmap &bm, const wxPoint &pos, const wxSize &size) {
    Create(parent, id, pos, size);
    setImage(bm);
    setBindings();
}

ImageWindow::ImageWindow(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size) {
    Create(parent, id, pos, size);
    setBindings();
}

void ImageWindow::setImage(wxBitmap &bm) {
    if(!bm.IsOk()) return;
    if(img) delete img;
    img = new wxBitmap(bm);
    SetMinSize(wxSize(img->GetSize().GetWidth(), img->GetSize().GetHeight()));
    SetSize(wxSize(img->GetSize().GetWidth(), img->GetSize().GetHeight()));
}

void ImageWindow::setBindings() {
    Bind(wxEVT_PAINT, &ImageWindow::onPaint, this);
}

void ImageWindow::updateImage(wxBitmap &bm) {
    setImage(bm);
}

void ImageWindow::onPaint(wxPaintEvent &event) {
    if(img) {
        wxPaintDC dev(this);
        dev.DrawBitmap(*img, 0, 0);
    }
}

ImageWindow::~ImageWindow() {
    if(img) delete img;
}
