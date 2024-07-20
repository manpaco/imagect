#ifndef CANVASITEM_H
#define CANVASITEM_H

#include "defs.hpp"
#include <wx/gdicmn.h>
#include <wx/geometry.h>
#include "smartrect.hpp"

class PixelView;
class Scaler;
class wxMemoryDC;
class ExtendedCanvas;

class CanvasItem {
public:
    CanvasItem();
    CanvasItem(int id, wxRect2DDouble geometry);

    int getId() const;
    virtual void drawOn(wxMemoryDC *pv);
    void lock(const bool opt);
    bool isLocked();
    void hide(bool opt);
    bool isHidden() const;
    wxRect2DDouble getGeometry(ict::ECContext ic, bool ext = true, bool ref = true) const;
    wxPoint2DDouble getPosition(ict::ECContext ic, bool ext = true, bool ref = true) const;
    wxPoint2DDouble getSize(ict::ECContext ic, bool ext = true) const;
    wxRect2DDouble getArea() const;
    wxRect2DDouble getHandleZone(int z) const;
    void setVirtualGeometry(const wxRect2DDouble &geo);
    void setVirtualPosition(const wxPoint2DDouble &pos);
    void setVirtualSize(const wxPoint2DDouble &dim);
    void select(const bool select);
    bool isSelected() const;
    void restrict(const bool state);
    bool isRestricted() const;
    void setVirtualRestriction(const wxRect2DDouble &restriction);
    wxDouble getAspectRatio() const;
    void setAspectRatio(int xr, int yr);
    void setScaler(Scaler *s);
    void setContainer(ExtendedCanvas *c);
    void expandFromCenter(bool op);
    bool expandFromCenter() const;
    void useGrid(bool);
    bool useGrid() const;

    /**
     * Enable or disable the fix aspect ratio.
     */
    void fixedAspectRatio(bool op);
    bool fixedAspectRatio() const;

    /**
     * Get the zone pressed.
     */
    int getHandler() const;

    bool operator==(const CanvasItem &);
    bool operator!=(const CanvasItem &);

    ~CanvasItem();

private:
    wxDouble extGetLeft() const;
    wxDouble extGetTop() const;
    wxDouble extGetRight() const;
    wxDouble extGetBottom() const;
    wxDouble extGetWidth() const;
    wxDouble extGetHeight() const;
    wxRect2DDouble extGetRect() const;

    wxDouble getWidth(ict::ECContext ic, bool ext = true) const;
    wxDouble getHeight(ict::ECContext ic, bool ext = true) const;
    wxDouble getRight(ict::ECContext ic, bool ext = true, bool ref = true) const;
    wxDouble getLeft(ict::ECContext ic, bool ext = true, bool ref = true) const;
    wxDouble getTop(ict::ECContext ic, bool ext = true, bool ref = true) const;
    wxDouble getBottom(ict::ECContext ic, bool ext = true, bool ref = true) const;

    void useSavedMark();

    /**
     * Modify the the item geometry.
     *
     * @param target Point used to move or resize.
     * @return true if crop rectangle changes, else false.
     */
    void modify(const wxPoint &p);
    void virtualModify(const wxPoint2DDouble &vp);

    /**
     * Simulate pressure at a given point.
     *
     * @param p point, used to calculate zone and offset in pressure.
     */
    int press(const wxPoint &avp);

    bool doHover(int z);

    wxRect2DDouble getAreaUpdate() const;
    wxRect2DDouble getHoverUpdate() const;

    /**
     * Release the simulated pressure.
     */
    void release();

    /**
     * Get offset from p to respective zone.
     */
    wxPoint2DDouble relativeToEdge(const wxPoint2DDouble &p, int z, ict::ECContext c, bool ext = true, bool ref = true);
    wxPoint2DDouble relativeToReference(const wxPoint2DDouble &p, ict::ECContext c);

    int inHandle(const wxPoint2DDouble &vp) const;

    void drawHandles(wxMemoryDC *pv);

    wxPoint2DDouble getContainerReference(ict::ECContext c) const;

    int id;
    bool selected;
    bool locked;
    bool hidden;
    int hover, sHover;
    wxPoint2DDouble rPressure, lastPoint;
    Scaler *scaler;
    SmartRect geometry;
    wxRect2DDouble sGeometry;
    ExtendedCanvas *container;

    /* Minimum handle dimension */
    wxDouble hdim;

    friend ExtendedCanvas;

};

#endif // !CANVASITEM_H
