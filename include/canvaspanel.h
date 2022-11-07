#ifndef CANVASPANEL_H
#define CANVASPANEL_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class CanvasPanel: public wxScrolledWindow {
    public:
        CanvasPanel(wxWindow *parent, wxWindowID id);
};

#endif // CANVASPANEL_H
