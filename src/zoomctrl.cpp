#include "zoomctrl.h"
#include "defs.h"
#include "wx/button.h"
#include "wx/textctrl.h"
#include "wx/sizer.h"
#include <wx/gdicmn.h>

ZoomCtrl::ZoomCtrl(wxWindow *parent, wxWindowID id) : wxControl(parent, id) {
    zIn = new wxButton(this, ict::ZIN_BT, _("+"));
    zOut = new wxButton(this, ict::ZOUT_BT, _("-"));
    percent = new wxTextCtrl(this, wxID_ANY, _("100%"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
    wxBoxSizer *zSizer = new wxBoxSizer(wxHORIZONTAL);
    zSizer->Add(zOut);
    zSizer->Add(percent);
    zSizer->Add(zIn);
    SetSizerAndFit(zSizer);
}
