#ifndef ICTAPP_H
#define ICTAPP_H

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

#endif // ICTAPP_H
