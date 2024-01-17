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

#include "imagewindow.hpp"
#include "wx/bitmap.h"
#include "wx/dcclient.h"

ImageWindow::ImageWindow(wxWindow *parent,
                         wxWindowID id,
                         wxBitmap &bm,
                         const wxPoint &pos,
                         const wxSize &size) {
    Create(parent, id, pos, size);
    updateImage(bm);
    setBindings();
}

ImageWindow::ImageWindow(wxWindow *parent,
                         wxWindowID id,
                         const wxPoint &pos,
                         const wxSize &size) {
    Create(parent, id, pos, size);
    setBindings();
}

bool ImageWindow::AcceptsFocus() const {
    return false;
}

void ImageWindow::setImage(wxBitmap &bm) {
    if(!bm.IsOk()) return;
    if(img) delete img;
    img = new wxBitmap(bm);
    SetMinSize(wxSize(img->GetSize().GetWidth(), img->GetSize().GetHeight()));
    SetSize(wxSize(img->GetSize().GetWidth(), img->GetSize().GetHeight()));
}

void ImageWindow::setBindings() {
    Bind(wxEVT_PAINT, &ImageWindow::onPaint, this);
}

void ImageWindow::updateImage(wxBitmap &bm) {
    setImage(bm);
    Refresh();
}

void ImageWindow::onPaint(wxPaintEvent &event) {
    if(img) {
        wxPaintDC dev(this);
        dev.DrawBitmap(*img, 0, 0);
    }
    event.Skip();
}

ImageWindow::~ImageWindow() {
    if(img) delete img;
}
