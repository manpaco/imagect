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

#include "defs.h"
#include <wx/gdicmn.h>

class PixelView;
class Scaler;
class wxMemoryDC;

class CanvasItem {
public:
    CanvasItem();
    CanvasItem(int id, wxRect geometry);

    int getId() const;
    virtual void drawOn(wxMemoryDC *pv);
    void lockEntries(const bool opt);
    bool lockEntries();
    wxRect getGeometry(const bool relativeToParent) const;
    wxPoint getPosition(const bool relativeToParent) const;
    wxSize getDimensions() const;
    wxRect getScaledGeometry(const bool relativeToParent) const;
    wxPoint getScaledPosition(const bool relativeToParent) const;
    wxSize getScaledDimensions() const;
    wxRect getScaledArea(const bool relativeToParent) const;
    bool setGeometry(const wxRect &geo);
    bool setPosition(const wxPoint &pos);
    bool setDimensions(const wxSize &dim);
    bool toggleSelection();
    void setSelection(const bool select);
    bool constraintOn() const;
    bool constraintState(const bool state);
    void setConstraint(const wxRect &constraint);
    void parentConstraint();
    double aspectRatio() const;
    void aspectRatio(int xr, int yr);
    void setParent(CanvasItem *p);
    void setScaler(Scaler *s);
    bool hasParent() const;

    /**
     * Update zones based in crop rectangle.
     */
    void updateScaledZones();

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
    bool modify(const wxPoint &target);

    /**
     * Simulate pressure at a given point.
     *
     * @param p point, used to calculate zone and offset in pressure.
     */
    ict::ItemZone press(const wxPoint &p);

    /**
     * Release the simulated pressure.
     */
    void release();

    /**
     * Get the zone pressed.
     */
    ict::ItemZone zonePressed() const;

    bool operator==(const CanvasItem &);
    bool operator!=(const CanvasItem &);

    ~CanvasItem();

private:
    bool applyGeometry(const wxRect &geo);

    /**
     * Move the crop rectangle such that it fits in the constraint.
     */
    void pushToConstraint();

    /**
     * Fit the crop rectangle in the constraint holding the position. That
     * means the size is modified.
     */
    void fitInConstraint();

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

    ict::ItemZone getLocation(const wxPoint &p) const;

    double unmodAspectRatio() const;

    void resetAccums();

    virtual void updateBuffer() { }

    void drawEntries(wxMemoryDC *pv);
    
    wxPoint relativeToScaledParent(wxPoint scaledPoint);

    int id;
    wxRect geometry;
    wxRect constraint;
    wxRect unmodGeo;
    bool selected;
    bool locked;
    bool fixed;
    bool conState;
    double accumX = 0.0, accumY = 0.0;
    ict::ItemZone zPressed;
    wxPoint relativePress;
    wxPoint lastPoint;
    wxRect scaledZones[ict::NUM_ZONES];

    CanvasItem *parent;
    Scaler *scaler;
};

#endif // !CANVASITEM_H
