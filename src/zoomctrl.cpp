#include "zoomctrl.h"
#include "wx/button.h"
#include "wx/textctrl.h"
#include "wx/sizer.h"
#include <wx/gdicmn.h>
#include "zoomevent.h"

ZoomCtrl::ZoomCtrl(wxWindow *parent, wxWindowID id) : 
    wxControl(parent, id, wxDefaultPosition, wxDefaultSize, wxNO_BORDER) {
    zIn = new wxButton(this, ict::ZIN_BT, _("+"), wxDefaultPosition,
                       wxDefaultSize, wxBU_EXACTFIT);
    zOut = new wxButton(this, ict::ZOUT_BT, _("-"), wxDefaultPosition,
                        zIn->GetSize());
    percent = new wxTextCtrl(this, wxID_ANY, _("100%"), wxDefaultPosition,
                             wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
    wxBoxSizer *zSizer = new wxBoxSizer(wxHORIZONTAL);
    zSizer->Add(zOut);
    zSizer->AddSpacer(ict::BEST_SPACE);
    zSizer->Add(percent);
    zSizer->AddSpacer(ict::BEST_SPACE);
    zSizer->Add(zIn);
    SetSizerAndFit(zSizer);
    zIt = &zValues.at(3);
    Bind(wxEVT_BUTTON, &ZoomCtrl::onZout, this, ict::ZOUT_BT);
    Bind(wxEVT_BUTTON, &ZoomCtrl::onZin, this, ict::ZIN_BT);
}

void ZoomCtrl::onZout(wxCommandEvent &event) {
    zIt--;
    if(zIt == zValues.begin()) zOut->Enable(false);
    if(!zIn->IsEnabled()) zIn->Enable();
    showPercent(*zIt);
    sendZoomEvent();
}

void ZoomCtrl::onZin(wxCommandEvent &event) {
    zIt++;
    ict::ZoomArray::const_iterator last = zValues.end();
    last--;
    if(zIt == last) zIn->Enable(false);
    if(!zOut->IsEnabled()) zOut->Enable();
    showPercent(*zIt);
    sendZoomEvent();
}

void ZoomCtrl::sendZoomEvent() {
    ZoomEvent toSend(EVT_ZOOM_CHANGE, GetId(), *zIt);
    toSend.SetEventObject(this);
    ProcessWindowEvent(toSend);
}

void ZoomCtrl::showPercent(float sf) {
    int p = sf * 100;
    std::string toShow = std::to_string(p) + "%";
    percent->SetValue(toShow);
}

ZoomCtrl::~ZoomCtrl() {

}
