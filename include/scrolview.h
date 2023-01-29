/**
 *     Copyright (C) 2023 Acuilán Gabriel <acuilangabriel@gmail.com>
 *
 *     This file is part of Image Cropping Tool.
 *
 *     Image Cropping Tool is free software: you can redistribute it and/or
 *     modify it under the terms of the GNU General Public License as published
 *     by the Free Software Foundation, either version 3 of the License, or (at
 *     your option) any later version.
 * 
 *     Image Cropping Tool is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 *     Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License along
 *     with Image Cropping Tool. If not, see <https://www.gnu.org/licenses/>.
 */

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
