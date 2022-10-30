#ifndef ICTAPP_H
#define ICTAPP_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class ImgCropApp: public wxApp {
    public:
        virtual bool OnInit();
};

#endif // ICTAPP_H
