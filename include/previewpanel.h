#ifndef PREVIEWPANEL_H
#define PREVIEWPANEL_H

class ImageWindow;
class wxStaticText;
class wxBoxSizer;
class wxGridSizer;

#include "wx/panel.h"

class PreviewPanel: public wxPanel {

    public:
        PreviewPanel(wxWindow *parent, wxWindowID id);
        PreviewPanel(wxWindow *parent, wxWindowID id, wxBitmap &);
        void updatePreview(wxBitmap &);
        void clear();
        ~PreviewPanel();

    private:
        void createSizer();
        void paintImage(wxPaintEvent &event); 
        void setBindings();
        void initParams();
        void updateElements(wxBitmap &bm);

        wxBoxSizer *sz = nullptr;
        wxGridSizer *imgSz = nullptr;
        ImageWindow *preview = nullptr;
        wxStaticText *title = nullptr;

};

#endif // PREVIEWPANEL_H
