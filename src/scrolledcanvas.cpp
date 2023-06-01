#include "scrolledcanvas.h"
#include "canvasitem.h"
#include "defs.h"
#include "scaler.h"
#include <wx/defs.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/scrolbar.h>

ScrolledCanvas::ScrolledCanvas(wxWindow *parent, wxWindowID id) : wxWindow(parent, id) {
    layout = new wxFlexGridSizer(2, 2, 0, 0);
    canvas = new wxPanel(this);
    vBar = new wxScrollBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);
    hBar = new wxScrollBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
    layout->Add(canvas);
    layout->Add(vBar);
    layout->Add(hBar);
    SetSizer(layout);
    scaler = new Scaler(1, 1, ict::FLOOR_ST);
    testRectangle = new CanvasItem(wxRect(0, 0, 40, 40), nullptr, scaler, false);
}

ScrolledCanvas::~ScrolledCanvas() {

}
