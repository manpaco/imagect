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
        ~ZoomCtrl();

    private:
        void onZout(wxCommandEvent &event);
        void onZin(wxCommandEvent &event);
        void sendZoomEvent();
        void showPercent(float sf);
        void initStacks();
        void checkStacks();

        wxButton *zIn, *zOut;
        wxTextCtrl *percent;

        bool custom;
        double current;
        std::stack<double> outStack, inStack;
};

#endif // ZOOMCTRL_H
