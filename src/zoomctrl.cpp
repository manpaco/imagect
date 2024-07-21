#include "zoomctrl.hpp"
#include "wx/button.h"
#include "wx/textctrl.h"
#include "wx/sizer.h"
#include <wx/gdicmn.h>
#include "zoomevent.hpp"
#include "defs.hpp"

ZoomCtrl::ZoomCtrl(wxWindow *parent, wxWindowID id) :
    wxControl(parent, id, wxDefaultPosition, wxDefaultSize, wxNO_BORDER) {
    zIn = new wxButton(this, ict::ZIN_BT, _("+"));
    zOut = new wxButton(this, ict::ZOUT_BT, _("-"));
    percent = new wxTextCtrl(this, wxID_ANY, _("NULL"), wxDefaultPosition,
                             wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
    wxBoxSizer *zSizer = new wxBoxSizer(wxHORIZONTAL);
    zSizer->Add(zOut);
    zSizer->AddSpacer(ict::BEST_SPACE);
    zSizer->Add(percent);
    zSizer->AddSpacer(ict::BEST_SPACE);
    zSizer->Add(zIn);
    SetSizerAndFit(zSizer);
    initStacks();
    checkStacks();
    showPercent(current);
    Bind(wxEVT_BUTTON, &ZoomCtrl::onZoomOut, this, ict::ZOUT_BT);
    Bind(wxEVT_BUTTON, &ZoomCtrl::onZoomIn, this, ict::ZIN_BT);
}

void ZoomCtrl::onZoomOut(wxCommandEvent &event) {
    inStack.push(current);
    current = outStack.top();
    outStack.pop();
    checkStacks();
    showPercent(current);
    sendZoomEvent();
}

void ZoomCtrl::onZoomIn(wxCommandEvent &event) {
    outStack.push(current);
    current = inStack.top();
    inStack.pop();
    checkStacks();
    showPercent(current);
    sendZoomEvent();
}

void ZoomCtrl::sendZoomEvent() {
    ZoomEvent toSend(EVT_ZOOM_CHANGE, GetId(), current);
    toSend.SetEventObject(this);
    ProcessWindowEvent(toSend);
}

void ZoomCtrl::showPercent(double factor) {
    double p = factor * 100;
    std::string toShow = std::to_string(p) + "%";
    percent->SetValue(toShow);
}

void ZoomCtrl::initStacks() {
    for(double factor = ict::MINUPP; factor < 1; factor *= 2) {
        outStack.push(factor);
    }
    for(double factor = ict::MAXUPP; factor > 1; factor /= 2) {
        inStack.push(factor);
    }
    current = 1;
}

void ZoomCtrl::checkStacks() {
    if(outStack.empty()) zOut->Disable();
    else zOut->Enable();
    if(inStack.empty()) zIn->Disable();
    else zIn->Enable();
}

ZoomCtrl::~ZoomCtrl() {

}
