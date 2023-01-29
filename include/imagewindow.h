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
