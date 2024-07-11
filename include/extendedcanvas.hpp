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
class Scaler;
class CanvasItem;

namespace ict {

    enum ItemOption {
        IO_NONE = 0,
        IO_LOCKED = 1,
        IO_HIDDEN = 2,
        IO_SELECTED = 3,
        IO_RESTRICTED = 4,
        IO_FIXEDASPECTRATIO = 5,
        IO_EXPANDFROMCENTER = 6
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
    void gridToggle(wxMouseEvent &event);
    void keyDown(wxKeyEvent &event);
    void keyUp(wxKeyEvent &event);
    void toggleItemOption(CanvasItem *item, ict::ItemOption option);
    void checkModKeys();

    void adjustScrollbars();
    wxRect getItemsCoverage();

    bool grid;
    bool shiftPressed, ctrlPressed;
    wxScrollBar *vBar, *hBar;
    wxPoint prevPosBars;
    wxFlexGridSizer *layout;
    wxWindow *canvas;
    wxWindow *zoom;
    Scaler *scaler;
    std::vector<CanvasItem *> zOrder;
    CanvasItem *pressedItem, *selectedItem, *hoveredItem;
    wxPoint2DDouble canvasReference;

    wxBitmap *canvasBuffer;

    friend CanvasItem;
};

#endif // !EXTENDEDCANVAS_H
