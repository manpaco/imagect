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

#ifndef IMAGECTAPP_H
#define IMAGECTAPP_H

class wxString;

#include "wx/app.h"

#if wxUSE_CMDLINE_PARSER
    #include <wx/cmdline.h>
#endif

class ImgCropApp: public wxApp {
    public:
        virtual bool OnInit();
        virtual void OnInitCmdLine(wxCmdLineParser &parser);
        virtual bool OnCmdLineParsed(wxCmdLineParser &parser);
        virtual int OnExit();

    private:
        wxString *cliFile = nullptr;
};

static const wxCmdLineEntryDesc cliDesc[] = {
    { wxCMD_LINE_PARAM,
        nullptr,
        nullptr,
        "input image",
        wxCMD_LINE_VAL_STRING,
        wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_NONE }
};

wxDECLARE_APP(ImgCropApp);

#endif // IMAGECTAPP_H
