#ifndef CANVASPANEL_H
#define CANVASPANEL_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/custombgwin.h"
#include "rectangle.h"

class CanvasPanel: public wxScrolledWindow {
    public:
        CanvasPanel(wxWindow *parent, wxWindowID id, const wxBitmap &);

    private:
        void createImg(const wxBitmap &bm);

        wxBoxSizer *sz;
        Rectangle *cropArea;
        wxCustomBackgroundWindow<wxPanel> *img;

};

#endif // CANVASPANEL_H
