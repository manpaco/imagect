#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class ImageWindow: public wxWindow {
    public:
        ImageWindow(wxWindow *parent, wxWindowID id, wxBitmap *bm, 
                const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize);
        ~ImageWindow();

    private:
        void onPaint(wxPaintEvent &);

        wxBitmap *bm;
};

#endif // IMAGEWINDOW_H
