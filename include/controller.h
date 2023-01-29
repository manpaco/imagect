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

#ifndef CONTROLLER_H
#define CONTROLLER_H

class wxSize;
class wxPoint;

#include "wx/gdicmn.h"
#include "defs.h"

/**
 * Implementation of a crop controller.
 * 
 * Manages the position and size of crop rectangle in a virtual space. This
 * contains simulated and direct modification functions.
 * Simulated: allows to simulate user inputs (press, release, move and resize).
 * Direct: allows to modify directly the crop rectangle (change size and pos.). 
 * Fix aspect ratio and constraint area are supported.
 */
class CropController {
    public:
        /**
         * Ctor.
         */
        CropController();

        /**
         * Create a controller with initial crop rectangle. The constraint will
         * be the same as the initial rectangle.
         */
        CropController(const wxRect &c);

        /**
         * Dtor.
         */
        ~CropController();
        
        /**
         * Change the crop size to given one. The constraint is used if its
         * active.
         *
         * @return true if crop size changes, else false.
         */
        bool cropSize(const wxSize &s);

        /**
         * Change the crop rectangle to given one. The constraint is used if
         * its active.
         *
         * @return true if crop rectangle changes, else false.
         */
        bool cropRect(const wxRect &r);

        /**
         * Enable or disable the constraint area.
         *
         * @return true if crop rectangle changes after enable constraint,
         *         else false.
         */
        bool constraint(bool op);

        /**
         * Set the constraint. This is used to delimit the area where the crop
         * rectangle can exist.
         *
         * @param cons Delimited area.
         */
        void constraint(wxRect &cons);

        /**
         * Modify the the crop rectangle. If inner zone is pressed the
         * rectangle is moved. Otherwise, the rectangle is resized.
         *
         * @param target Point used to move or resize.
         * @return true if crop rectangle changes, else false.
         */
        bool modify(const wxPoint &target);

        /**
         * Simulate pressure at a given zone.
         *
         * @param z Zone to press.
         * @param p Pressure point, used to calculate offset in pressure.
         */
        void press(const ict::Zone z, const wxPoint &p);

        /**
         * Release the simulated pressure.
         */
        void release();

        /**
         * Enable or disable the fix aspect ratio.
         */
        void fixRatio(bool op);

        /**
         * Get the crop size in virtual canvas.
         */
        wxSize cropSize() const;

        /**
         * Get the crop position in virtual canvas.
         */
        wxPoint cropPosition() const;

        /**
         * Get the crop rectangle in virtual canvas.
         */
        wxRect cropRect() const;

        /**
         * Get the zone pressed.
         */
        ict::Zone zonePressed() const;

        /**
         * Ask if the constraint is active.
         */
        bool constraint() const;

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
         * Internal function that allows to change the crop rectangle and hold
         * the ratio. It also allows to set new accumulators.
         *          
         * @return true if crop rectangle changes, else false.
         */
        bool cropRect(const wxRect &r,
                      bool holdRatio,
                      float initAx,
                      float initAy);

        /**
         * Set the ratio used by the accumulators to calculate deltas. This
         * does not modify the crop rectangle.
         */
        void setRatio(float r);

        /**
         * Update zones based in crop rectangle.
         */
        void updateZones();

        /**
         * Get offset from p to respective zone.
         */
        wxPoint relativeToZone(const wxPoint &p, ict::Zone z);

        wxRect crop;
        float ratio;
        bool fix = false;
        float accumX = 0.0, accumY = 0.0;
        wxRect cons;
        bool activeCons = false;
        ict::Zone fixHint, pressZone;

        wxPoint relativePress;
        wxRect viz, vnz, vsz, vez, vwz, vnez, vnwz, vsez, vswz;

};

#endif // CONTROLLER_H
