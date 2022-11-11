#ifndef PREVIEWPANEL_H
#define PREVIEWPANEL_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/custombgwin.h"

class PreviewPanel: public wxScrolledCanvas {

    public:
        PreviewPanel(wxWindow *parent, wxWindowID id);
        void updatePreview(wxBitmap *);
        ~PreviewPanel();

    private:
        void paintImage(wxPaintEvent &event); 

        wxBitmap *preview;
        wxCustomBackgroundWindow<wxWindow> *frame;
        wxBoxSizer *sz;
};

#endif // PREVIEWPANEL_H
