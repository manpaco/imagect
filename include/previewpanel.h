#ifndef PREVIEWPANEL_H
#define PREVIEWPANEL_H

#include "Magick++/Image.h"
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/custombgwin.h"

class PreviewPanel: public wxScrolledWindow {

    public:
        PreviewPanel(wxWindow *parent, wxWindowID id);
        void updatePreview(Magick::Image &img);

    private:
        void paintImage(wxPaintEvent &event); 
        void createBitmap(Magick::Image &img);

        wxBitmap *bm;
        wxCustomBackgroundWindow<wxPanel> *box;
        wxBoxSizer *sz;
};

#endif // PREVIEWPANEL_H
