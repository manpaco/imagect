#ifndef ZOOMCTRL_H
#define ZOOMCTRL_H

class wxButton;
class wxTextCtrl;

#include "wx/control.h"

class ZoomCtrl : public wxControl {
    public:
        ZoomCtrl(wxWindow *parent, wxWindowID id);
    private:
        float zValues[6] = {0.5, 0.75, 1.0, 1.5, 2.5, 5.0};
        wxButton *zIn, *zOut;
        wxTextCtrl *percent;
};

#endif // ZOOMCTRL_H
