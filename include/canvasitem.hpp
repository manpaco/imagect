/**
 *     Copyright (C) 2023 Acuilán Gabriel <acuilangabriel@gmail.com>
 *
 *     This file is part of ImageCT.
 *
 *     ImageCT is free software: you can redistribute it and/or modify it under
 *     the terms of the GNU General Public License as published by the Free
 *     Software Foundation, either version 3 of the License, or (at your
 *     option) any later version.
 *
 *     ImageCT is distributed in the hope that it will be useful, but WITHOUT
 *     ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *     FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *     more details.
 *
 *     You should have received a copy of the GNU General Public License along
 *     with ImageCT. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CANVASITEM_H
#define CANVASITEM_H

#include "defs.hpp"
#include <wx/gdicmn.h>
#include <wx/geometry.h>

enum ItemContext {
    VIRTUAL_CONTEXT,
    CANVAS_CONTEXT
};

class PixelView;
class Scaler;
class wxMemoryDC;

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
    wxDouble getX(ItemContext ic, bool unref = false) const;
    wxDouble getY(ItemContext ic, bool unref = false) const;
    wxDouble getWidth(ItemContext ic) const;
    wxDouble getHeight(ItemContext ic) const;
    wxRect2DDouble getGeometry(ItemContext ic, bool unref = false) const;
    wxPoint2DDouble getPosition(ItemContext ic, bool unref = false) const;
    wxPoint2DDouble getDimensions(ItemContext ic) const;
    wxRect2DDouble getArea() const;
    wxRect2DDouble getZone(ict::ItemZone z) const;
    bool setVirtualGeometry(const wxRect2DDouble &geo);
    bool setVirtualPosition(const wxPoint2DDouble &pos);
    bool setVirtualDimensions(const wxPoint2DDouble &dim);
    bool toggleSelection();
    void select(const bool select);
    bool isSelected() const;
    bool restrict(const bool state);
    bool isRestricted() const;
    void setVirtualRestriction(const wxRect2DDouble &restriction);
    wxDouble getAspectRatio() const;
    void setAspectRatio(int xr, int yr);
    void setScaler(Scaler *s);
    void setVirtualReference(wxPoint2DDouble *r);
    wxPoint2DDouble getReference(ItemContext c);

    /**
     * Enable or disable the fix aspect ratio.
     */
    void fixAspectRatio(bool op);

    /**
     * Modify the the item geometry.
     *
     * @param target Point used to move or resize.
     * @return true if crop rectangle changes, else false.
     */
    bool modify(const wxPoint &avp);

    /**
     * Simulate pressure at a given point.
     *
     * @param p point, used to calculate zone and offset in pressure.
     */
    ict::ItemZone press(const wxPoint &avp);

    /**
     * Release the simulated pressure.
     */
    void release();

    /**
     * Get the zone pressed.
     */
    ict::ItemZone getZonePressed() const;

    bool operator==(const CanvasItem &);
    bool operator!=(const CanvasItem &);

    ~CanvasItem();

private:
    wxDouble getRight(ItemContext ic, bool unref = false) const;
    wxDouble getLeft(ItemContext ic, bool unref = false) const;
    wxDouble getTop(ItemContext ic, bool unref = false) const;
    wxDouble getBottom(ItemContext ic, bool unref = false) const;

    bool applyGeometry(const wxRect2DDouble &geo);

    /**
     * Move the crop rectangle such that it fits in the constraint.
     */
    void pushToRestriction();

    /**
     * Fit the crop rectangle in the constraint holding the position. That
     * means the size is modified.
     */
    void fitInRestriction(ict::ItemZone simulation);

    /**
     * Move to target point. Relative pressure is considered.
     */
    void move(const wxPoint2DDouble &t);

    /**
     * Resize to target point. Relative pressure and edge zones are
     * considered.
     */
    void resize(const wxPoint2DDouble &t);

    /**
     * Get offset from p to respective zone.
     */

    wxPoint2DDouble relativeToEdge(const wxPoint2DDouble &p, ict::ItemZone z, ItemContext c);

    void inflateRect(wxRect2DDouble &r) const;

    void modifyPosition(wxDouble x, wxDouble y);
    void modifySize(wxDouble w, wxDouble h);

    ict::ItemZone getLocation(const wxPoint2DDouble &vp) const;

    wxDouble getUnmodAspectRatio() const;

    bool exceedsBottomRestriction() const;
    bool exceedsTopRestriction() const;
    bool exceedsRightRestriction() const;
    bool exceedsLeftRestriction() const;

    void drawEntries(wxMemoryDC *pv);

    int id;
    wxRect2DDouble geometry;
    wxRect2DDouble restriction;
    wxRect2DDouble unmodGeometry;
    bool selected;
    bool locked;
    bool fixed;
    bool restricted;
    bool hidden;
    ict::ItemZone zonePressed;
    wxPoint2DDouble relativePress;
    wxPoint2DDouble lastPoint;
    Scaler *scaler;
    wxPoint2DDouble *reference;
};

#endif // !CANVASITEM_H
