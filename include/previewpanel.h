#ifndef PREVIEWPANEL_H
#define PREVIEWPANEL_H

class ImageWindow;
class wxStaticText;
class wxBoxSizer;
class wxGridSizer;

#include "wx/panel.h"

/**
 * Implementation of a preview panel
 *
 * Handle one image centered in the panel. Preview title.
 * Support update.
 */
class PreviewPanel: public wxPanel {

    public:
        /**
         * Ctor.
         */
        PreviewPanel(wxWindow *parent, wxWindowID id);

        /**
         * Construct a preview handling the given image.
         */
        PreviewPanel(wxWindow *parent, wxWindowID id, wxBitmap &);

        /**
         * Update the handled image.
         */
        void updatePreview(wxBitmap &);

        /**
         * Delete the internal image.
         */
        void clear();

        /**
         * Dtor.
         */
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
