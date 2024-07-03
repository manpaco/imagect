#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H

#include "wx/window.h"

/**
 * Implementation of a image container.
 *
 * This allow to manage the image with sizers.
 */
class ImageWindow: public wxWindow {
    public:
        ImageWindow(wxWindow *parent,
                    wxWindowID id,
                    wxBitmap &bm,
                    const wxPoint &pos=wxDefaultPosition,
                    const wxSize &size=wxDefaultSize);
        ImageWindow(wxWindow *parent,
                    wxWindowID id,
                    const wxPoint &pos=wxDefaultPosition,
                    const wxSize &size=wxDefaultSize);

        /**
         * Update contained image.
         */
        void updateImage(wxBitmap &bm);

        /**
         * Ask if accepts focus.
         */
        bool AcceptsFocus() const;
        ~ImageWindow();

    private:
        void onPaint(wxPaintEvent &);
        void setImage(wxBitmap &);
        void setBindings();

        wxBitmap *img = nullptr;
};

#endif // IMAGEWINDOW_H
