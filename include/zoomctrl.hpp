#ifndef ZOOMCTRL_H
#define ZOOMCTRL_H

class wxButton;
class wxTextCtrl;

#include "wx/control.h"
#include <stack>

/**
 * Implementation of a zoom control
 *
 * Manages the zoom changes sending events.
 * Offers buttons and display box.
 */
class ZoomCtrl : public wxControl {
    public:
        ZoomCtrl(wxWindow *parent, wxWindowID id);
        void setCustom(const double &factor);
        ~ZoomCtrl();

    private:
        void onZoomOut(wxCommandEvent &event);
        void onZoomIn(wxCommandEvent &event);
        void sendZoomEvent();
        void updatePercent();
        void initStacks();
        void checkStacks();
        void dumpToInStack();

        wxButton *zIn, *zOut;
        wxTextCtrl *percent;

        bool custom;
        double current;
        std::stack<double> outStack, inStack;
};

#endif // ZOOMCTRL_H
