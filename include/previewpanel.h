/**
 *     Copyright (C) 2023 Acuilán Gabriel <acuilangabriel@gmail.com>
 *
 *     This file is part of ImageCT.
 *
 *     ImageCT is free software: you can redistribute it and/or modify it under
 *     the terms of the GNU General Public License as published by the Free
 *     Software Foundation, either version 3 of the License, or (at your
 *     option) any later version.
 * 
 *     ImageCT is distributed in the hope that it will be useful, but WITHOUT
 *     ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *     FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *     more details.
 *
 *     You should have received a copy of the GNU General Public License along
 *     with ImageCT. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PREVIEWPANEL_H
#define PREVIEWPANEL_H

class ImageWindow;
class wxStaticText;
class wxBoxSizer;
class wxGridSizer;
class wxBitmap;

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
