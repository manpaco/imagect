#ifndef ZOOMCTRL_H
#define ZOOMCTRL_H

class wxButton;
class wxTextCtrl;

#include "wx/control.h"
#include <array>

class ZoomCtrl : public wxControl {
    public:
        ZoomCtrl(wxWindow *parent, wxWindowID id);
        ~ZoomCtrl();

    private:
        wxButton *zIn, *zOut;
        wxTextCtrl *percent;
};

static const std::array<float, 8> zValues = {0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0};

#endif // ZOOMCTRL_H
