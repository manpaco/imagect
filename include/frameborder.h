#ifndef FRAMEBORDER_H
#define FRAMEBORDER_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class FrameBorder : public wxControl {
    public:
        FrameBorder();

    protected:
        void Init();
        void OnPaint(wxPaintEvent &);

    private:
        wxDECLARE_DYNAMIC_CLASS(FrameBorder);
        wxSize area;

};

#endif // FRAMEBORDER_H
