/**
 *     Copyright (C) 2023 Acuilán Gabriel <acuilangabriel@gmail.com>
 *
 *     This file is part of imagect.
 *
 *     imagect is free software: you can redistribute it and/or modify it under
 *     the terms of the GNU General Public License as published by the Free
 *     Software Foundation, either version 3 of the License, or (at your
 *     option) any later version.
 *
 *     imagect is distributed in the hope that it will be useful, but WITHOUT
 *     ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *     FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *     more details.
 *
 *     You should have received a copy of the GNU General Public License along
 *     with imagect. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SMARTRECT
#define SMARTRECT

#include <wx/geometry.h>
class Scaler;

namespace ict {

    /* enum RectZone {
        NONE_ZONE = 0,
        IN_ZONE,
        T_ZONE,
        RT_ZONE,
        LT_ZONE,
        B_ZONE,
        RB_ZONE,
        LB_ZONE,
        R_ZONE,
        L_ZONE,
    }; */

    /* enum Reflection {
        NONE_REFLEC = 0,
        HORI_REFLEC,
        VERT_REFLEC,
        HOVE_REFLEC
    }; */

    const int NONE_REFLEC = 0;
    const int HORI_REFLEC = 1;
    const int VERT_REFLEC = 2;
    const int HOVE_REFLEC = 3;

    const int NONE_ZONE = 0;
    const int IN_ZONE = 1;
    const int RB_ZONE = 4;
    const int LB_ZONE = 5;
    const int RT_ZONE = 6;
    const int LT_ZONE = 7;
    const int R_ZONE = 8;
    const int L_ZONE = 9;
    const int T_ZONE = 12;
    const int B_ZONE = 14;

}

class SmartRect : public wxRect2DDouble {
public:
    SmartRect();
    /* SmartRect(SmartRect &&);
    SmartRect(const SmartRect &); */
    SmartRect(const wxRect2DDouble &);

    bool setGeometry(const wxRect2DDouble &r);
    bool setPosition(const wxPoint2DDouble &r);
    bool setSize(const wxPoint2DDouble &r);

    bool pushZoneTo(const int z, const wxPoint2DDouble &p);

    bool restrict(const bool r);
    bool setRestriction(const wxRect2DDouble &r);
    void setMark();
    void useMark();
    void fixedAspectRatio(const bool ar);
    void setAspectRatio(const wxDouble &ar);
    void setAspectRatio(const int x, const int y);
    void expandFromCenter(const bool ec);
    wxDouble getAspectRatio() const;
    wxRect2DDouble inflatedRect(const wxDouble &x, const wxDouble &y) const;
    void useGrid(bool);

    bool isFixed() const;
    bool isCentered() const;
    bool isRestricted() const;
    int getReflection() const;
    bool useGrid() const;
    int getLastZone() const;
    wxRect2DDouble getChangeUnion() const;

    /* SmartRect &operator=(SmartRect &&) = default;
    SmartRect &operator=(const SmartRect &) = default; */
    SmartRect &operator=(const wxRect2DDouble &);
    ~SmartRect();

private:
    bool pushTo(const wxPoint2DDouble &p);
    bool pushTopTo(const wxDouble &p);
    bool pushRightTopTo(const wxPoint2DDouble &p);
    bool pushRightTo(const wxDouble &p);
    bool pushRightBottomTo(const wxPoint2DDouble &p);
    bool pushBottomTo(const wxDouble &p);
    bool pushLeftBottomTo(const wxPoint2DDouble &p);
    bool pushLeftTo(const wxDouble &p);
    bool pushLeftTopTo(const wxPoint2DDouble &p);

    wxDouble internalGetLeft() const;
    wxDouble internalGetTop() const;
    wxDouble internalGetRight() const;
    wxDouble internalGetBottom() const;
    wxPoint2DDouble internalGetCentre() const;

    void saveInstant();
    bool instantChanged();

    void modifyGeometry(wxDouble x, wxDouble y, wxDouble w, wxDouble h);
    void modifyPosition(wxDouble x, wxDouble y);
    void modifySize(wxDouble w, wxDouble h);

    wxRect2DDouble getPlayground() const;
    wxRect2DDouble getInnerPlayground() const;
    void placeInPlayground(wxPoint2DDouble *p, bool inner = false) const;

    wxDouble calcAbsc(const wxDouble &ordi) const;
    wxDouble calcOrdi(const wxDouble &absc) const;

    wxDouble accumulateLeft(const wxDouble &dl);
    wxDouble accumulateTop(const wxDouble &dt);
    wxDouble accumulateRight(const wxDouble &dr);
    wxDouble accumulateBottom(const wxDouble &db);
    void clearAccums();

    void assembleGrid();
    void disassembleGrid();

    /* ----------------------- SmartRect main structure -----------------------
     * External getters are inherited from wxRect2DDouble.
     * Internal getters are identified by "internal" prefix.
     * Getters that don't have "internal" prefix will be external.
     *
     * When grid is enabled: external =/= internal, most of the time.
     * When grid is disabled: external == internal, always. */

    wxRect2DDouble restriction;
    wxRect2DDouble mark;
    wxRect2DDouble instant;
    wxDouble aspectRatio;

    /* Geometry accumulators used when resizing and grid is active.
     * These accumulators save decimals of the respective border.
     * i. e. if "internalGetLeft" is 15.34, then Left is 15 and accLeft is 0.34
     * They are not affected by the "move" function (when IN_ZONE is used)
     * since it only affects the POSITION (external).
     * POSITION =/= "internalGetLeftTop"; when grilled
     * i. e. "internalGetLeftTop" == (GetLeft + accLeft, GetTop + accTop)
     * then, POSITION == (GetLeft, GetTop) */
    wxDouble accLeft, accTop, accRight, accBottom;

    bool fixed;
    bool centered;
    bool restricted;
    bool grid;

    int reflection = ict::NONE_REFLEC;
    int lastZone;
};

#endif // !SMARTRECT
