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

#ifndef EXPORTDLG_H
#define EXPORTDLG_H

#include "wx/filedlg.h"
#include "defs.h"

/**
 * Implementation of a export dialog.
 *
 * Generic wxFileDialog with custom wildcards and path validation.
 */
class ExportDialog: public wxFileDialog {
    public:
        /**
         * Ctor.
         */
        ExportDialog(wxWindow *parent);

        /**
         * Return the path selected by the user with valid extension.
         */
        wxString validPath();

        /**
         * Dtor.
         */
        ~ExportDialog();

    private:
        /**
         * Initialize wildcards
         */
        void initWildcards();

        std::string imageWcs[ict::WILDCARD_SIZE];
        std::string exportWc;
};

#endif // EXPORTDLG_H
