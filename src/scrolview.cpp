#include "scrolview.h"
#include "canvaspanel.h"
#include "wx/sizer.h"
#include <iostream>

ScrolledView::ScrolledView(wxWindow *parent, wxWindowID id) {
    Create(parent, id);
    initParams();
}

ScrolledView::~ScrolledView() {

}

void ScrolledView::scaleFactor(float sf) {
    if(canvas) canvas->setScaleFactor(sf); 
    Layout();
}

void ScrolledView::clear() {
    if(canvas) {
        centerSizer->Detach(canvas);
        canvas->Destroy();
        canvas = nullptr;
    }
}

void ScrolledView::handle(Magick::Image *img) {
    if(canvas) clear();
    canvas = new CanvasPanel(this, ict::CANVAS, img);
    centerSizer->Add(canvas, 0, wxALIGN_CENTER | wxALL);
    Layout();
}

CanvasPanel * ScrolledView::getCanvas() const {
    return canvas;
}

void ScrolledView::initParams() {
    SetScrollRate(ict::CORNER, ict::CORNER);
    centerSizer = new wxGridSizer(1, 0, 0);
    SetSizer(centerSizer);
}
