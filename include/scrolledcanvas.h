#ifndef SCROLLEDCANVAS_H
#define SCROLLEDCANVAS_H

#include <wx/window.h>

class ScrolledCanvas : public wxWindow {
public:
    ScrolledCanvas();
    ScrolledCanvas(ScrolledCanvas &&);
    ScrolledCanvas(const ScrolledCanvas &);
    ScrolledCanvas &operator=(ScrolledCanvas &&);
    ScrolledCanvas &operator=(const ScrolledCanvas &);
    ~ScrolledCanvas();

private:
    
};

#endif // !SCROLLEDCANVAS_H
