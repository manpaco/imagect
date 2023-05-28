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
#include <wx/geometry.h>
#include <array>

class CanvasItem {
public:
    CanvasItem();
    CanvasItem(wxRect geometry, CanvasItem *parent, Scaler *scaler, bool locked = true);

    virtual void drawOn(CanvasPortion *cp) { }
    void lockEntries(const bool opt);
    bool lockEntries();
    void disconnect(const bool opt);
    bool disconnect();
    wxRect getGeometry() const;
    wxPoint getPosition() const;
    wxSize getDimensions() const;
    wxRect getScaledGeometry(const bool relativeToParent) const;
    wxPoint getScaledPosition(const bool relativeToParent) const;
    wxSize getScaledDimensions() const;
    bool setGeometry(const wxRect &geo);
    bool setPosition(const wxPoint &pos);
    bool setDimensions(const wxSize &dim);
    bool toggleSelection();
    void setSelection(const bool select);
    bool isKey() const;
    bool constraintOn() const;
    bool constraintState(const bool state);
    void setConstraint(const wxRect &constraint);
    void parentConstraint();

    void doMagnify(wxPoint canvasCenter, wxPoint mouseDelta);
    void doScroll(wxPoint motion);

    /**
     * Enable or disable the fix aspect ratio.
     */
    void fixRatio(bool op);

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

    ~CanvasItem();

private:
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
     * Update zones based in crop rectangle.
     */
    void updateScaledZones();

    /**
     * Get offset from p to respective zone.
     */
    wxPoint relativeToEdge(const wxPoint &p, ict::ItemZone z);

    ict::ItemZone getLocation(const wxPoint &p) const;

    float unmodRatio() const;

    void resetAccums();

    virtual void updateBuffer() { }
    void notifyChange(const wxRect &damaged);
    void drawEntries();

    wxRect geometry;
    wxRect constraint;
    wxRect unmodGeo;
    bool selected;
    bool locked;
    bool disconnected;
    bool fixed;
    bool conState;
    float accumX = 0.0, accumY = 0.0;
    ict::ItemZone zPressed;
    wxPoint relativePress;
    wxPoint lastPoint;
    std::array<wxRect, ict::NUM_ZONES> scaledZones;

    CanvasItem *parent;
    CanvasPortion *tempPortion;
    Scaler *scaler;
};

#endif // !CANVASITEM_H
