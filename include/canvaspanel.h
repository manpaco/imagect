#ifndef CANVASPANEL_H
#define CANVASPANEL_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "frameborder.h"

class CanvasPanel: public wxScrolledWindow {
    public:
        CanvasPanel(wxWindow *parent, wxWindowID id);
        void mouseMotionHandler(wxMouseEvent &);
        void mouseLeftPress(wxMouseEvent &);
        void mouseLeftRelease(wxMouseEvent &);

    private:
        wxButton *test;
        wxBoxSizer *sz;

        bool mousePressCrop = false;
};

#endif // CANVASPANEL_H
