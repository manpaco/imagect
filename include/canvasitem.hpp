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
    CanvasItem(int id, wxRect geometry);

    int getId() const;
    virtual void drawOn(wxMemoryDC *pv);
    void lock(const bool opt);
    bool isLocked();
    void hide(bool opt);
    bool isHidden() const;
    int getX(ItemContext ic) const;
    int getY(ItemContext ic) const;
    int xVirtualUnref() const;
    int yVirtualUnref() const;
    int getWidth(ItemContext ic) const;
    int getHeight(ItemContext ic) const;
    wxRect getGeometry(ItemContext ic) const;
    wxPoint getPosition(ItemContext ic) const;
    wxSize getDimensions(ItemContext ic) const;
    wxRect getArea() const;
    wxRect getZone(ict::ItemZone z) const;
    bool setVirtualGeometry(const wxRect &geo);
    bool setVirtualPosition(const wxPoint &pos);
    bool setVirtualDimensions(const wxSize &dim);
    bool toggleSelection();
    void select(const bool select);
    bool isSelected() const;
    bool restrict(const bool state);
    bool isRestricted() const;
    void setVirtualRestriction(const wxRect &restriction);
    double getAspectRatio() const;
    void setAspectRatio(int xr, int yr);
    void setScaler(Scaler *s);
    void setOffset(wxPoint vo);
    wxPoint getOffset() const;
    void setCanvasReference(CanvasItem *r);

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
    int getRight(ItemContext ic) const;
    int getLeft(ItemContext ic) const;
    int getTop(ItemContext ic) const;
    int getBottom(ItemContext ic) const;

    bool applyGeometry(const wxRect &geo);

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
     * Use delta in y axis to calculate delta in x axis. Here the ratio is
     * considered
     */
    void accumulateX(int &dxToCalc, int &dyToUse);

    /**
     * Use delta in x axis to calculate delta in y axis. Here the ratio is
     * considered
     */
    void accumulateY(int &dyToCalc, int &dxToUse);

    /**
     * Move to target point. Relative pressure is considered.
     */
    void move(const wxPoint &t);

    /**
     * Resize to target point. Relative pressure and edge zones are
     * considered.
     */
    void resize(const wxPoint &t);

    /**
     * Get offset from p to respective zone.
     */

    wxPoint relativeToEdge(const wxPoint &p, ict::ItemZone z);

    ict::ItemZone getLocation(const wxPoint &vp) const;

    double getUnmodAspectRatio() const;

    void resetAccums();

    void drawEntries(wxMemoryDC *pv);

    int id;
    wxRect geometry;
    wxRect restriction;
    wxRect unmodGeometry;
    bool selected;
    bool locked;
    bool fixed;
    bool restricted;
    bool hidden;
    double accumX = 0.0, accumY = 0.0;
    ict::ItemZone zonePressed;
    wxPoint relativePress;
    wxPoint lastPoint;
    wxPoint offset;
    Scaler *scaler;
    CanvasItem *reference;
};

#endif // !CANVASITEM_H
