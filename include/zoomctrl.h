#ifndef ZOOMCTRL_H
#define ZOOMCTRL_H

class wxButton;
class wxTextCtrl;

#include "wx/control.h"
#include "defs.h"

static const ict::ZoomArray zValues = {
    0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0
};

class ZoomCtrl : public wxControl {
    public:
        ZoomCtrl(wxWindow *parent, wxWindowID id);
        ~ZoomCtrl();

    private:
        void onZout(wxCommandEvent &event);
        void onZin(wxCommandEvent &event);
        void sendZoomEvent();
        void showPercent(float sf);

        ict::ZoomArray::const_iterator zIt;
        wxButton *zIn, *zOut;
        wxTextCtrl *percent;
};

#endif // ZOOMCTRL_H
