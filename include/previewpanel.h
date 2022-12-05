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
        void clear();
        ~PreviewPanel();

    private:
        void paintImage(wxPaintEvent &event); 
        void setBindings();
        void initDimensions();
        void updateElements(wxBitmap &bm);
        void tryToAttachImg();

        wxBoxSizer *sz = nullptr;
        ImageWindow *preview = nullptr;
        wxStaticText *title = nullptr;

        bool attachedImg = false;
};

#endif // PREVIEWPANEL_H
