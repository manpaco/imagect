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

#include "exportdlg.hpp"
#include "defs.hpp"
#include "filext.hpp"

ExportDialog::ExportDialog(wxWindow *parent):
    wxFileDialog(parent, _("Export image"), wxEmptyString, wxEmptyString,
                 wxEmptyString, wxFD_SAVE | wxFD_OVERWRITE_PROMPT) {
    initWildcards();
    SetWildcard(exportWc);
}

wxString ExportDialog::validPath() {
    wxString fn = GetFilename();
    wxString p = GetPath();
    if(!extension(fn.ToStdString())) {
        switch(GetFilterIndex()) {
        case ict::PNG_WC:
            p.Append(pngExt);
            break;
        case ict::JPEG_WC:
            p.Append(jpgExt);
            break;
        }
    }
    return p;
}

void ExportDialog::initWildcards(){
    imageWcs[ict::PNG_WC] = pngWc;
    imageWcs[ict::JPEG_WC] = jpegWc;
    exportWc = imageWcs[0];
    for(int i = 1; i < ict::WILDCARD_SIZE; i++) {
        exportWc += "|" + imageWcs[i];
    }
}

ExportDialog::~ExportDialog() {

}
