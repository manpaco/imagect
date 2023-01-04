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

void ScrolledView::handle(CanvasPanel *cp) {
    if(canvas) clear();
    canvas = cp;
    centerSizer->Add(canvas, 0, wxALIGN_CENTER | wxALL);
    Layout();
}

void ScrolledView::initParams() {
    SetScrollRate(ict::CORNER, ict::CORNER);
    centerSizer = new wxGridSizer(1, 0, 0);
    SetSizer(centerSizer);
}
