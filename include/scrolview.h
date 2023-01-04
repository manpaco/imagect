#ifndef SCROLVIEW_H
#define SCROLVIEW_H

class CanvasPanel;
class wxGridSizer;

#include "wx/scrolwin.h"

class ScrolledView : public wxScrolledCanvas {
    public:
        ScrolledView(wxWindow *parent, wxWindowID id);
        ~ScrolledView();

        void scaleFactor(float sf);
        void clear();
        void handle(CanvasPanel *cp);

    private:
        void initParams();

        CanvasPanel *canvas = nullptr;
        wxGridSizer *centerSizer = nullptr;
};

#endif // SCROLVIEW_H
