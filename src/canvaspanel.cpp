#include "canvaspanel.h"
#include "defs.h"

CanvasPanel::CanvasPanel(wxWindow *parent, wxWindowID id) : 
        wxScrolledWindow() {
    Create(parent, id);

    test = new wxButton(this, ict::CROP_RECT, "TEST");
    sz = new wxBoxSizer(wxVERTICAL);
    sz->Add(test);
    SetSizerAndFit(sz);
    test->Bind(wxEVT_MOTION, &CanvasPanel::mouseMotionHandler, this);
    test->Bind(wxEVT_LEFT_DOWN, &CanvasPanel::mouseLeftPress, this);
    test->Bind(wxEVT_LEFT_UP, &CanvasPanel::mouseLeftRelease, this);
}

void CanvasPanel::mouseMotionHandler(wxMouseEvent &event) {
    if(mousePressCrop) {
        // repaint crop rectangle
        std::cout << "moving" << std::endl;
    }
    event.Skip();
}

void CanvasPanel::mouseLeftPress(wxMouseEvent &event) {
    mousePressCrop = true;
    std::cout << "press" << std::endl;
}

void CanvasPanel::mouseLeftRelease(wxMouseEvent &event) {
    mousePressCrop = false;
    std::cout << "release" << std::endl;
}
