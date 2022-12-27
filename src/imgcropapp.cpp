#include "imgcropapp.h"
#include "wx/string.h"
#include "mainframe.h"
#include <Magick++.h>

wxIMPLEMENT_APP(ImgCropApp);

bool ImgCropApp::OnInit() {
    if(!wxApp::OnInit()) return false;

#ifdef __WXMSW__
    Magick::InitializeMagick(wxGetCwd().char_str());
#endif // __WXMSW__

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
