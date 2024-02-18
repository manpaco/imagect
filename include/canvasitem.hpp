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
#include "smartrect.hpp"
#include "defs.hpp"

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
    wxDouble getX(ict::ECContext ic, bool unref = false) const;
    wxDouble getY(ict::ECContext ic, bool unref = false) const;
    wxDouble getWidth(ict::ECContext ic) const;
    wxDouble getHeight(ict::ECContext ic) const;
    wxRect2DDouble getGeometry(ict::ECContext ic, bool unref = false) const;
    wxPoint2DDouble getPosition(ict::ECContext ic, bool unref = false) const;
    wxPoint2DDouble getDimensions(ict::ECContext ic) const;
    wxRect2DDouble getArea() const;
    wxRect2DDouble getZone(int z) const;
    bool setVirtualGeometry(const wxRect2DDouble &geo);
    bool setVirtualPosition(const wxPoint2DDouble &pos);
    bool setVirtualDimensions(const wxPoint2DDouble &dim);
    bool toggleSelection();
    void select(const bool select);
    bool isSelected() const;
    bool restrict(const bool state);
    bool isRestricted() const;
    bool setVirtualRestriction(const wxRect2DDouble &restriction);
    wxDouble getAspectRatio() const;
    void setAspectRatio(int xr, int yr);
    void setScaler(Scaler *s);
    void setContainer(ExtendedCanvas *c);
    void expandFromCenter(bool op);
    void useGrid(bool);
    bool useGrid() const;

    /**
     * Enable or disable the fix aspect ratio.
     */
    void fixedAspectRatio(bool op);

    /**
     * Get the zone pressed.
     */
    int getPressedZone() const;

    bool operator==(const CanvasItem &);
    bool operator!=(const CanvasItem &);

    ~CanvasItem();

private:
    wxDouble getRight(ict::ECContext ic, bool unref = false) const;
    wxDouble getLeft(ict::ECContext ic, bool unref = false) const;
    wxDouble getTop(ict::ECContext ic, bool unref = false) const;
    wxDouble getBottom(ict::ECContext ic, bool unref = false) const;

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
    int press(const wxPoint &avp);

    void hoverCollision();
    void hover(int z);
    bool collides(const wxPoint2DDouble &);

    wxRect2DDouble getUpdateArea() const;
    wxRect2DDouble getHoverUpdate() const;

    /**
     * Release the simulated pressure.
     */
    void release();

    /**
     * Get offset from p to respective zone.
     */
    wxPoint2DDouble relativeToEdge(const wxPoint2DDouble &p, int z, ict::ECContext c);

    int getLocation(const wxPoint2DDouble &vp) const;

    void drawEntries(wxMemoryDC *pv);

    wxPoint2DDouble getContainerReference(ict::ECContext c) const;

    int id;
    bool selected;
    bool locked;
    bool hidden;
    int pressedZone, hoverZone, prevHover, collisionZone;
    wxPoint2DDouble relativePress;
    wxPoint2DDouble lastPoint;
    Scaler *scaler;
    SmartRect geometry;
    ExtendedCanvas *container;

    friend ExtendedCanvas;

};

#endif // !CANVASITEM_H
