#ifndef EXTENDEDCANVAS_H
#define EXTENDEDCANVAS_H

#include <vector>
#include <wx/event.h>
#include <wx/window.h>
#include <wx/geometry.h>
#include "defs.hpp"

class wxScrollBar;
class wxFlexGridSizer;
class wxPanel;
class wxCheckBox;
class Scaler;
class CanvasItem;
class ZoomCtrl;
class ZoomEvent;

namespace ict {

    enum ItemOption {
        IOPT_NONE = 0,
        IOPT_LOCK = 1,
        IOPT_HIDE = 2,
        IOPT_SELECT = 3,
        IOPT_RESTRICT = 4,
        IOPT_FIXEDASPECTRATIO = 5,
        IOPT_EXPANDFROMCENTER = 6
    };
}

class ExtendedCanvas : public wxWindow {
public:
    ExtendedCanvas(wxWindow *parent, wxWindowID id);

    void addItem(CanvasItem *item);
    CanvasItem * getItem(int itemId);
    wxPoint2DDouble getReference(ict::ECContext c) const;
    void useGrid(bool);
    bool useGrid() const;
    bool hasItems() const;

    ~ExtendedCanvas();

private:
    void paintCanvas(wxPaintEvent &event);
    void mouseMotion(wxMouseEvent &event);
    void mousePress(wxMouseEvent &event);
    void mouseRelease(wxMouseEvent &event);
    void resizeCanvas(wxSizeEvent &event);
    void mouseWheel(wxMouseEvent &event);
    void horizontalScroll(wxScrollEvent &event);
    void verticalScroll(wxScrollEvent &event);
    void doMagnify(const wxPoint magCenter);
    void doScroll(const wxPoint motion);
    bool pressCanvas(const wxPoint p);
    bool hoverCanvas(const wxPoint p);
    void refreshCanvas();
    void refreshCanvasRect(const wxRect &r);
    void notifyGeometry(CanvasItem *changed);
    void notifySelection(CanvasItem *changed);
    void notifyPressure(CanvasItem *pressed);
    void notifyHover(CanvasItem *changed);
    void gridToggle(wxCommandEvent &event);
    void keyDown(wxKeyEvent &event);
    void keyUp(wxKeyEvent &event);
    void onZoomChange(ZoomEvent &event);
    void toggleOption(CanvasItem *item, ict::ItemOption option);
    void checkModKeys();
    void initScrollbars();
    void setItemsGrid(bool state);

    void adjustScrollbars();
    wxRect getItemsCoverage();

    bool shiftPressed, ctrlPressed;
    wxScrollBar *vBar, *hBar;
    wxPoint prevPosBars;
    wxFlexGridSizer *layout;
    wxWindow *canvas;
    ZoomCtrl *zoom;
    wxCheckBox *gridBox;
    Scaler *scaler;
    std::vector<CanvasItem *> zOrder;
    CanvasItem *pressedItem, *selectedItem;
    wxPoint2DDouble canvasReference;

    wxBitmap *canvasBuffer;

    friend CanvasItem;
};

#endif // !EXTENDEDCANVAS_H
