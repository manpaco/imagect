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

inline double round_htz(const double &n) {
    return std::copysign(ceil((fabs(n) - 0.5)), n);
}
inline double round_haz(const double &n) {
    return std::copysign(floor((fabs(n) + 0.5)), n);
}
inline int reflectionChange(const int &pReflec, const int &cReflec) {
    return pReflec ^ cReflec;
}

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

    wxDouble extGetLeft() const;
    wxDouble extGetTop() const;
    wxDouble extGetRight() const;
    wxDouble extGetBottom() const;
    wxDouble extGetWidth() const;
    wxDouble extGetHeight() const;

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
    void useGrid(bool);
    void saveBefore(bool);
    void setDummySpace(const wxDouble &, const wxDouble &);
    void clearDummySpace();

    bool isFixed() const;
    bool isCentered() const;
    bool isRestricted() const;
    int getReflection() const;
    bool useGrid() const;
    int getLastZone() const;
    wxRect2DDouble getChangeUnion() const;

    /* SmartRect &operator=(SmartRect &&) = default;
    SmartRect &operator=(const SmartRect &) = default; */
    ~SmartRect();

private:
    void pushTo(const wxPoint2DDouble &p);
    void pushTopTo(const wxDouble &p);
    void pushRightTopTo(const wxPoint2DDouble &p);
    void pushRightTo(const wxDouble &p);
    void pushRightBottomTo(const wxPoint2DDouble &p);
    void pushBottomTo(const wxDouble &p);
    void pushLeftBottomTo(const wxPoint2DDouble &p);
    void pushLeftTo(const wxDouble &p);
    void pushLeftTopTo(const wxPoint2DDouble &p);

    void saveInstant();
    bool instantChanged();

    wxRect2DDouble getCenteredPlayground() const;
    wxRect2DDouble getMovePlayground() const;
    wxRect2DDouble getResizePlayground(int) const;
    void placeInsidePlayground(wxPoint2DDouble *p, int z) const;

    wxDouble calcAbsc(const wxDouble &ordi) const;
    wxDouble calcOrdi(const wxDouble &absc) const;

    bool restrictionContains(const wxPoint2DDouble &p) const;
    wxDouble restrictionLeft() const;
    wxDouble restrictionTop() const;
    wxDouble restrictionRight() const;
    wxDouble restrictionBottom() const;
    wxDouble restrictionLeftLimit() const;
    wxDouble restrictionTopLimit() const;
    wxDouble restrictionRightLimit() const;
    wxDouble restrictionBottomLimit() const;
    wxRect2DDouble restrictionGeometryLimit() const;

    /* ----------------------- SmartRect main structure -----------------------
     * Internal getters are inherited from wxRect2DDouble.
     * External getters are identified by "ext" prefix.
     *
     * When grid is enabled: external =/= internal, most of the time.
     * When grid is disabled: external == internal, always. */

    /* Restriction values */
    wxDouble rx, ry, rw, rh;
    /* Mark values (to save current values and use them later)*/
    wxDouble mx, my, mw, mh;
    /* Instant values (previous values, before last push) */
    wxDouble ix, iy, iw, ih;
    /* Aspect ratio value */
    wxDouble aspectRatio;
    /* Space used as dummy stripe in reflections */
    wxDouble dummyX, dummyY;

    bool grid;
    bool fixed;
    bool centered;
    bool restricted;
    bool save;

    int reflection;
    int lastZone;
};

#endif // !SMARTRECT
