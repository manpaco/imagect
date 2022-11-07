#include "canvaspanel.h"
#include <wx/generic/panelg.h>

CanvasPanel::CanvasPanel(wxWindow *parent, wxWindowID id) : 
        wxScrolledWindow() {
    Create(parent, id);
}
