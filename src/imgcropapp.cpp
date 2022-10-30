#include "imgcropapp.h"
#include "mainframe.h"

wxIMPLEMENT_APP(ImgCropApp);

bool ImgCropApp::OnInit() {
    MainFrame *imgCropFrame = new MainFrame();
    imgCropFrame->Show(true);
    
    return true;
}
