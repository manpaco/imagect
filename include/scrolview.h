#ifndef SCROLVIEW_H
#define SCROLVIEW_H

class CanvasPanel;
class wxGridSizer;
namespace Magick {
    class Image;
}

#include "wx/scrolwin.h"

/**
 * Implementation of a scrolleable view
 *
 * Manages the scroll and zoom behaviour.
 */
class ScrolledView : public wxScrolledCanvas {
    public:
        /**
         * Ctor.
         */
        ScrolledView(wxWindow *parent, wxWindowID id);

        /**
         * Dtor.
         */
        ~ScrolledView();

        /**
         * Set the zoom of the view.
         */
        void scaleFactor(float sf);

        /**
         * Delete the internal canvas panel.
         */
        void clear();

        /**
         * Handle a given image, creating the internal canvas panel.
         */
        void handle(Magick::Image *img);

        /**
         * Get the pointer to internal canvas panel.
         */
        CanvasPanel * getCanvas() const;

    private:
        void initParams();

        CanvasPanel *canvas = nullptr;
        wxGridSizer *centerSizer = nullptr;
};

#endif // SCROLVIEW_H
