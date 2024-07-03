#ifndef SMARTRECT
#define SMARTRECT

#define ROUND_CORRECTOR 0.5f
#define SMALLER_RECT 0
#define BIGGER_RECT 1

#include <wx/geometry.h>
class Scaler;

inline double round_htz(const double &n) {
    return std::copysign(ceil((fabs(n) - ROUND_CORRECTOR)), n);
}
inline double round_haz(const double &n) {
    return std::copysign(floor((fabs(n) + ROUND_CORRECTOR)), n);
}
inline double round_rb(const double &n) {
    if(n < 0) return round_htz(n);
    else return round_haz(n);
}
inline double round_lb(const double &n) {
    if(n > 0) return round_htz(n);
    else return round_haz(n);
}
inline int reflectionChange(const int &pReflec, const int &cReflec) {
    return pReflec ^ cReflec;
}

namespace ict {

    /* Reflections */
    const int NONE_REFLEC = 0;
    const int HORI_REFLEC = 1;
    const int VERT_REFLEC = 2;
    const int HOVE_REFLEC = 3;

    /* Non-reflectable zones */
    const int NONE_ZONE = 0;
    const int IN_ZONE = 1;
    const int NEW_ZONE = 2;
    /* Reserved zone to indicate begining of resizing zones */
    const int RES_ZONE = 3;
    /* Reflectable zones
     * i.e. RB_ZONE ^ HORI_REFLEC == LB_ZONE */
    const int RB_ZONE = 4;
    const int LB_ZONE = 5;
    const int RT_ZONE = 6;
    const int LT_ZONE = 7;
    /* HORI_REFLEC works on R_ZONE and L_ZONE
     * VERT_REFLEC works on T_ZONE and B_ZONE
     * Any other combination may result in unexpected behavior */
    const int R_ZONE = 8;
    const int L_ZONE = 9;
    const int T_ZONE = 12;
    const int B_ZONE = 14;

    /* Clamps
     * Use bitwise OR to clamp, bitwise AND to check
     * and bitwise XOR to toggle */
    const int NONE_CLAMPED = 0;
    const int R_CLAMPED = 1 << 0;
    const int L_CLAMPED = 1 << 1;
    const int T_CLAMPED = 1 << 2;
    const int B_CLAMPED = 1 << 3;
    const int RT_CLAMPED = R_CLAMPED | T_CLAMPED;
    const int LT_CLAMPED = L_CLAMPED | T_CLAMPED;
    const int RB_CLAMPED = R_CLAMPED | B_CLAMPED;
    const int LB_CLAMPED = L_CLAMPED | B_CLAMPED;

}

class SmartRect : public wxRect2DDouble {
public:
    SmartRect();
    /* SmartRect(SmartRect &&);
    SmartRect(const SmartRect &); */
    SmartRect(const wxRect2DDouble &);

    void setGeometry(const wxRect2DDouble &r);
    void setPosition(const wxPoint2DDouble &r);
    void setSize(const wxPoint2DDouble &r);

    wxDouble extGetLeft() const;
    wxDouble extGetTop() const;
    wxDouble extGetRight() const;
    wxDouble extGetBottom() const;
    wxDouble extGetWidth() const;
    wxDouble extGetHeight() const;
    wxRect2DDouble extGetRect() const;

    void activateZone(const int z);
    void setZoneTo(const wxPoint2DDouble &p);

    void restrict(const bool r);
    void setRestriction(const wxRect2DDouble &r);
    void setMark();
    void useMark();
    void fixedAspectRatio(const bool ar);
    void setAspectRatio(const wxDouble &ar);
    void setAspectRatio(const int x, const int y);
    void useInflate(const bool i);
    void useGrid(bool);

    bool isFixed() const;
    bool useInflate() const;
    bool isRestricted() const;
    int getReflection() const;
    bool useGrid() const;
    wxDouble getAspectRatio() const;
    int activatedZone() const;
    bool cornerActivated() const;
    bool edgeActivated() const;
    bool resizing() const;
    bool dragging() const;
    bool resting() const;
    bool creating() const;

    /* SmartRect &operator=(SmartRect &&) = default;
    SmartRect &operator=(const SmartRect &) = default; */
    ~SmartRect();

private:

    wxDouble calcAbsc(const wxDouble &ordi) const;
    wxDouble calcOrdi(const wxDouble &absc) const;

    wxDouble leftRestriction() const;
    wxDouble topRestriction() const;
    wxDouble rightRestriction() const;
    wxDouble bottomRestriction() const;
    wxDouble leftRestrictionLimit() const;
    wxDouble topRestrictionLimit() const;
    wxDouble rightRestrictionLimit() const;
    wxDouble bottomRestrictionLimit() const;
    wxRect2DDouble restrictionLimits() const;

    void checkMinimum();
    void checkInflate();
    void checkReflection();
    void checkAspectRatio(int want = BIGGER_RECT);
    void checkRestriction();
    void checkLimits(bool doBalance = false);
    void checkGrid();
    void balance(int zone);
    int clampedZone() const;

    /* ----------------------- SmartRect main structure -----------------------
     * Internal getters are inherited from wxRect2DDouble.
     * External getters are identified by "ext" prefix.
     *
     * When grid is enabled: external =/= internal, most of the time.
     * When grid is disabled: external == internal, always. */

    /* Restriction values */
    wxRect2DDouble restriction;
    /* Mark values (to save current values and use them later for checks) */
    wxRect2DDouble mark;
    /* Aspect ratio value */
    wxDouble aspectRatio;

    bool grid;
    bool fixed;
    bool restricted;
    bool inflate;

    int reflection;
    int activeZone;
    int clamps;
    int markZone;
};

#endif // !SMARTRECT
