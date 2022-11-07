#ifndef CANVASPANEL_H
#define CANVASPANEL_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "rectangle.h"

class CanvasPanel: public wxScrolledWindow {
    public:
        CanvasPanel(wxWindow *parent, wxWindowID id);

    private:
        wxBoxSizer *sz;
        Rectangle *cropArea;

};

#endif // CANVASPANEL_H
