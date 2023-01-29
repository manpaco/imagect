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

#include "scrolview.h"
#include "canvaspanel.h"
#include "wx/sizer.h"
#include <iostream>

ScrolledView::ScrolledView(wxWindow *parent, wxWindowID id) {
    Create(parent, id);
    initParams();
}

ScrolledView::~ScrolledView() {

}

void ScrolledView::scaleFactor(float sf) {
    if(canvas) canvas->setScaleFactor(sf); 
    Layout();
}

void ScrolledView::clear() {
    if(canvas) {
        centerSizer->Detach(canvas);
        canvas->Destroy();
        canvas = nullptr;
    }
}

void ScrolledView::handle(Magick::Image *img) {
    if(canvas) clear();
    canvas = new CanvasPanel(this, ict::CANVAS, img);
    centerSizer->Add(canvas, 0, wxALIGN_CENTER | wxALL);
    Layout();
}

CanvasPanel * ScrolledView::getCanvas() const {
    return canvas;
}

void ScrolledView::initParams() {
    SetScrollRate(ict::CORNER, ict::CORNER);
    centerSizer = new wxGridSizer(1, 0, 0);
    SetSizer(centerSizer);
}
