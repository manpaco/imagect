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

#include "previewpanel.h"
#include "defs.h"
#include "imagewindow.h"
#include "wx/sizer.h"
#include "wx/stattext.h"

#if wxUSE_STATLINE
    #include <wx/statline.h>
#endif

PreviewPanel::PreviewPanel(wxWindow *parent, wxWindowID id) {
    Create(parent, id);
    createSizer();
    initParams();
}

PreviewPanel::
PreviewPanel(wxWindow *parent, wxWindowID id, wxBitmap &bm):
    PreviewPanel(parent, id) {
    updateElements(bm);
}

void PreviewPanel::createSizer() {
    sz = new wxBoxSizer(wxVERTICAL);
    title = new wxStaticText(this, wxID_ANY, "Preview");
    sz->Add(title, 0, wxALIGN_CENTER);
    sz->AddSpacer(ict::BEST_SPACE);
    sz->Add(new wxStaticLine(this), 0, wxEXPAND);
    imgSz = new wxGridSizer(1, 0, 0);
    sz->Add(imgSz, 1, wxEXPAND);
    SetSizer(sz);
}

void PreviewPanel::clear() {
    if(preview) {
        imgSz->Detach(preview);
        preview->Destroy();
        preview = nullptr;
    }
}

void PreviewPanel::initParams() {
    SetMinSize(wxSize(150, 150));
}

void PreviewPanel::updateElements(wxBitmap &bm) {
    if(!bm.IsOk()) return;
    if(!preview) {
        preview = new ImageWindow(this, wxID_ANY, bm);
        imgSz->Add(preview, 0, wxALIGN_CENTER);
    } else preview->updateImage(bm);
    Layout();
}

void PreviewPanel::updatePreview(wxBitmap &bm) {
    updateElements(bm);
}

PreviewPanel::~PreviewPanel() {

}
