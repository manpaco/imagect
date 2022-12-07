#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class ImageWindow: public wxWindow {
    public:
        ImageWindow(wxWindow *parent, wxWindowID id, wxBitmap &bm, 
                const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize);
        ImageWindow(wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize);
        void updateImage(wxBitmap &bm);
        bool AcceptsFocus() const;
        ~ImageWindow();

    private:
        void onPaint(wxPaintEvent &);
        void setImage(wxBitmap &);
        void setBindings();

        wxBitmap *img = nullptr;
};

#endif // IMAGEWINDOW_H
