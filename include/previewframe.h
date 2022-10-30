#ifndef PREVIEWFRAME_H
#define PREVIEWFRAME_H

#include "Magick++/Image.h"
#include <wx/event.h>
#include <wx/image.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class PreviewFrame: public wxFrame {

    public:
        PreviewFrame(wxWindow *parent, wxWindowID id, const wxString &title, Magick::Image &img);
        void updatePreview(Magick::Image &img);

    private:
        void paintImage(wxPaintEvent &event); 
        void createAdapter(Magick::Image &img);

        wxImage adapter;
};

#endif // PREVIEWFRAME_H
