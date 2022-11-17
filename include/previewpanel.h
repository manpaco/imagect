#ifndef PREVIEWPANEL_H
#define PREVIEWPANEL_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/custombgwin.h"
#include "imagewindow.h"

class PreviewPanel: public wxPanel {

    public:
        PreviewPanel(wxWindow *parent, wxWindowID id);
        PreviewPanel(wxWindow *parent, wxWindowID id, wxBitmap &);
        void updatePreview(wxBitmap &);
        ~PreviewPanel();

    private:
        void createElements(wxBitmap &bm);
        void paintImage(wxPaintEvent &event); 
        void setBindings();
        void init();
        void initDimensions();
        void setPreview(wxBitmap &bm);

        wxBoxSizer *sz = nullptr;
        ImageWindow *preview = nullptr;
        wxStaticText *title = nullptr;
};

#endif // PREVIEWPANEL_H
