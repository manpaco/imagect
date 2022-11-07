#ifndef RECTSASH_H
#define RECTSASH_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

enum Zone {
    Edge,
    Inner
};

class RectSash : public wxControl {
    public:
        RectSash();

    protected:
        void Init();
        void OnPaint(wxPaintEvent &);

    private:
        wxDECLARE_DYNAMIC_CLASS(RectSash);
        wxSize area;

};

#endif // RECTSASH_H
