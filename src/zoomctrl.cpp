#include "zoomctrl.hpp"
#include "wx/button.h"
#include "wx/textctrl.h"
#include "wx/sizer.h"
#include <wx/gdicmn.h>
#include "zoomevent.hpp"
#include "defs.hpp"
#include <sstream>

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
    updatePercent();
    Bind(wxEVT_BUTTON, &ZoomCtrl::onZoomOut, this, ict::ZOUT_BT);
    Bind(wxEVT_BUTTON, &ZoomCtrl::onZoomIn, this, ict::ZIN_BT);
}

void ZoomCtrl::setCustom(const double &factor) {
    if(factor < ict::MINUPP || factor > ict::MAXUPP) return;
    dumpToInStack();
    bool setted = false;
    while(!inStack.empty() && !setted) {
        if(factor > inStack.top()) {
            outStack.push(inStack.top());
            inStack.pop();
        } else {
            if(factor == inStack.top()) inStack.pop();
            else custom = true;
            current = factor;
            setted = true;
        }
    }
    checkStacks();
    updatePercent();
}

void ZoomCtrl::dumpToInStack() {
    if(!custom) inStack.push(current);
    else custom = false;
    while(!outStack.empty()) {
        inStack.push(outStack.top());
        outStack.pop();
    }
}

void ZoomCtrl::onZoomOut(wxCommandEvent &event) {
    if(!custom) inStack.push(current);
    else custom = false;
    current = outStack.top();
    outStack.pop();
    checkStacks();
    updatePercent();
    sendZoomEvent();
}

void ZoomCtrl::onZoomIn(wxCommandEvent &event) {
    if(!custom) outStack.push(current);
    else custom = false;
    current = inStack.top();
    inStack.pop();
    checkStacks();
    updatePercent();
    sendZoomEvent();
}

void ZoomCtrl::sendZoomEvent() {
    ZoomEvent toSend(EVT_ZOOM_CHANGE, GetId(), current);
    toSend.SetEventObject(this);
    ProcessWindowEvent(toSend);
}

void ZoomCtrl::updatePercent() {
    double p = current * 100;
    std::ostringstream toShow;
    toShow.precision(2);
    toShow << std::fixed << p;
    percent->SetValue(std::move(toShow).str());
}

void ZoomCtrl::initStacks() {
    for(double factor = ict::MINUPP; factor < 1; factor *= 2) {
        outStack.push(factor);
    }
    for(double factor = ict::MAXUPP; factor > 1; factor /= 2) {
        inStack.push(factor);
    }
    current = 1;
    custom = false;
}

void ZoomCtrl::checkStacks() {
    if(outStack.empty()) zOut->Disable();
    else zOut->Enable();
    if(inStack.empty()) zIn->Disable();
    else zIn->Enable();
}

ZoomCtrl::~ZoomCtrl() {

}
