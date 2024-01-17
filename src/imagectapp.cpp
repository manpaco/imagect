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

#include "imagectapp.hpp"
#include "wx/string.h"
#include "mainframe.hpp"

wxIMPLEMENT_APP(ImgCropApp);

bool ImgCropApp::OnInit() {
    if(!wxApp::OnInit()) return false;

    MainFrame *imgCropFrame;

    if(!cliFile) imgCropFrame = new MainFrame();
    else imgCropFrame = new MainFrame(*cliFile);
    imgCropFrame->Show(true);

    return true;
}

void ImgCropApp::OnInitCmdLine(wxCmdLineParser &parser) {
    parser.SetDesc(cliDesc);
}

bool ImgCropApp::OnCmdLineParsed(wxCmdLineParser &parser) {
    if(parser.GetParamCount() > 0) cliFile = new wxString(parser.GetParam());
    return true;
}

int ImgCropApp::OnExit() {
    if(cliFile) delete cliFile;
    return 0;
}
