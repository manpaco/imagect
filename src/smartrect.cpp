#include "smartrect.hpp"
#include "defs.hpp"
#include <iostream>
#include <iomanip>

SmartRect::SmartRect() : SmartRect(wxRect2DDouble(0, 0, 1, 1)) {

}

SmartRect::SmartRect(const wxRect2DDouble &r) {
    wxRect2DDouble::operator=(r);
    aspectRatio = m_width / m_height;
    fixed = false;
    restricted = false;
    clamps = ict::NONE_CLAMPED;
    inflate = false;
    grid = false;
    reflection = ict::NONE_REFLEC;
    activeZone = ict::NONE_ZONE;
    restriction = *this;
    setMark();
}

wxDouble SmartRect::leftRestriction() const {
    if(useGrid()) return round_rb(restriction.GetLeft());
    else return restriction.GetLeft();
}

wxDouble SmartRect::topRestriction() const {
    if(useGrid()) return round_rb(restriction.GetTop());
    else return restriction.GetTop();
}

wxDouble SmartRect::rightRestriction() const {
    if(useGrid()) return round_lb(restriction.GetRight());
    else return restriction.GetRight();
}

wxDouble SmartRect::bottomRestriction() const {
    if(useGrid()) return round_lb(restriction.GetBottom());
    else return restriction.GetBottom();
}

wxDouble SmartRect::leftRestrictionLimit() const {
    if(useGrid()) return leftRestriction() - ROUND_CORRECTOR;
    else return leftRestriction();
}

wxDouble SmartRect::topRestrictionLimit() const {
    if(useGrid()) return topRestriction() - ROUND_CORRECTOR;
    else return topRestriction();
}

wxDouble SmartRect::rightRestrictionLimit() const {
    if(useGrid()) return rightRestriction() + ROUND_CORRECTOR;
    else return rightRestriction();
}

wxDouble SmartRect::bottomRestrictionLimit() const {
    if(useGrid()) return bottomRestriction() + ROUND_CORRECTOR;
    else return bottomRestriction();
}

wxRect2DDouble SmartRect::restrictionLimits() const {
    return wxRect2DDouble(leftRestrictionLimit(), topRestrictionLimit(),
                          rightRestrictionLimit() - leftRestrictionLimit(),
                          bottomRestriction() - topRestrictionLimit());
}

void SmartRect::setGeometry(const wxRect2DDouble &r) {
    if(!resting()) return;
    setPosition(r.GetPosition());
    bool aux = inflate;
    if(aux) inflate = !aux;
    setSize(wxPoint2DDouble(r.m_width, r.m_height));
    inflate = aux;
}

void SmartRect::setPosition(const wxPoint2DDouble &p) {
    if(!resting()) return;
    activateZone(ict::IN_ZONE);
    setZoneTo(p);
    activateZone(ict::NONE_ZONE);
}

void SmartRect::setSize(const wxPoint2DDouble &s) {
    if(!resting()) return;
    wxPoint2DDouble newp(s);
    newp.m_x = abs(newp.m_x);
    newp.m_y = abs(newp.m_y);
    if(useInflate()) {
        newp.m_x /= 2;
        newp.m_y /= 2;
        newp += GetCentre();
    } else newp += GetPosition();
    activateZone(ict::RB_ZONE);
    setZoneTo(newp);
    activateZone(ict::NONE_ZONE);
}

wxDouble SmartRect::extGetLeft() const {
    if(useGrid()) return round_rb(GetLeft());
    else return GetLeft();
}

wxDouble SmartRect::extGetTop() const {
    if(useGrid()) return round_rb(GetTop());
    else return GetTop();
}

wxDouble SmartRect::extGetRight() const {
    if(useGrid()) return round_lb(GetRight());
    else return GetRight();
}

wxDouble SmartRect::extGetBottom() const {
    if(useGrid()) return round_lb(GetBottom());
    else return GetBottom();
}

wxDouble SmartRect::extGetWidth() const {
    if(useGrid()) return extGetRight() - extGetLeft();
    else return m_width;
}

wxDouble SmartRect::extGetHeight() const {
    if(useGrid()) return extGetBottom() - extGetTop();
    else return m_height;
}

wxRect2DDouble SmartRect::extGetRect() const {
    return wxRect2DDouble(extGetLeft(), extGetTop(),
                          extGetWidth(), extGetHeight());
}

void SmartRect::useGrid(bool op) {
    grid = op;
    restrict(restricted);
}

bool SmartRect::useGrid() const {
    return grid;
}

wxDouble SmartRect::getAspectRatio() const {
    return aspectRatio;
}

int SmartRect::getReflection() const {
    return reflection;
}

void SmartRect::setZoneTo(const wxPoint2DDouble &p) {
    if(resting()) return;
    switch(activeZone) {
        case ict::R_ZONE:
        case ict::RT_ZONE:
        case ict::RB_ZONE:
            SetRight(p.m_x);
            break;
        case ict::L_ZONE:
        case ict::LT_ZONE:
        case ict::LB_ZONE:
            SetLeft(p.m_x);
            break;
        case ict::IN_ZONE:
            MoveLeftTo(p.m_x);
            break;
    }
    switch(activeZone) {
        case ict::T_ZONE:
        case ict::LT_ZONE:
        case ict::RT_ZONE:
            SetTop(p.m_y);
            break;
        case ict::B_ZONE:
        case ict::LB_ZONE:
        case ict::RB_ZONE:
            SetBottom(p.m_y);
            break;
        case ict::IN_ZONE:
            MoveTopTo(p.m_y);
            break;
    }
    checkMinimum();
    checkInflate();
    checkReflection();
    checkAspectRatio();
    checkRestriction();
    checkGrid();
    if(!fixedAspectRatio()) setAspectRatio(m_width / m_height);
}

int SmartRect::activatedZone() const {
    return activeZone;
}

void SmartRect::activateZone(const int z) {
    activeZone = z;
    if(activeZone) setMark();
}

bool SmartRect::cornerActivated() const {
    return activeZone > ict::RES_ZONE && activeZone <= ict::LT_ZONE;
}

bool SmartRect::edgeActivated() const {
    return activeZone >= ict::R_ZONE && activeZone <= ict::B_ZONE;
}

bool SmartRect::resizing() const {
    return activeZone > ict::RES_ZONE;
}

bool SmartRect::dragging() const {
    return activeZone == ict::IN_ZONE;
}

bool SmartRect::resting() const {
    return activeZone == ict::NONE_ZONE;
}

bool SmartRect::creating() const {
    return activeZone == ict::NEW_ZONE;
}

void SmartRect::checkGrid() {
    if(!useGrid() || !dragging()) return;
    wxPoint2DDouble dp(GetPosition() - mark.GetPosition());
    dp.m_x = trunc(dp.m_x);
    dp.m_y = trunc(dp.m_y);
    m_x = mark.GetPosition().m_x + dp.m_x;
    m_y = mark.GetPosition().m_y + dp.m_y;
}

void SmartRect::checkLimits(bool doBalance) {
    if(!restricted) return;
    if(!dragging() && !resizing()) return;
    clamps = ict::NONE_CLAMPED;
    bool exceed = GetLeft() < leftRestrictionLimit();
    if(exceed) {
        if(dragging()) MoveLeftTo(leftRestrictionLimit());
        else {
            SetLeft(leftRestrictionLimit());
            clamps |= ict::L_CLAMPED;
        }
    }
    exceed = GetTop() < topRestrictionLimit();
    if(exceed) {
        if(dragging()) MoveTopTo(topRestrictionLimit());
        else {
            SetTop(topRestrictionLimit());
            clamps |= ict::T_CLAMPED;
        }
    }
    exceed = GetRight() > rightRestrictionLimit();
    if(exceed) {
        if(dragging()) MoveRightTo(rightRestrictionLimit());
        else {
            SetRight(rightRestrictionLimit());
            clamps |= ict::R_CLAMPED;
        }
    }
    exceed = GetBottom() > bottomRestrictionLimit();
    if(exceed) {
        if(dragging()) MoveBottomTo(bottomRestrictionLimit());
        else {
            SetBottom(bottomRestrictionLimit());
            clamps |= ict::B_CLAMPED;
        }
    }
    if(!clamps || dragging()) return;
    if((clamps & ict::R_CLAMPED) && (clamps & ict::L_CLAMPED)) {
        if(GetRight() - mark.GetCentre().m_x <
                mark.GetCentre().m_x - GetLeft())
            clamps ^= ict::L_CLAMPED;
        else clamps ^= ict::R_CLAMPED;
    }
    if((clamps & ict::T_CLAMPED) && (clamps & ict::B_CLAMPED)) {
        if(GetBottom() - mark.GetCentre().m_y <
                mark.GetCentre().m_y - GetTop())
            clamps ^= ict::T_CLAMPED;
        else clamps ^= ict::B_CLAMPED;
    }
    if(!doBalance) return;
    if(useInflate()) {
        balance(clampedZone());
        return;
    }
    int collateral = ict::NONE_ZONE;
    if(activeZone == ict::R_ZONE || activeZone == ict::L_ZONE) {
        if(clamps & ict::T_CLAMPED) collateral = ict::T_ZONE;
        else if(clamps & ict::B_CLAMPED) collateral = ict::B_ZONE;
    } else if(activeZone == ict::T_ZONE || activeZone == ict::B_ZONE) {
        if(clamps & ict::R_CLAMPED) collateral = ict::R_ZONE;
        else if(clamps & ict::L_CLAMPED) collateral = ict::L_ZONE;
    }
    if(collateral) balance(collateral);
}

int SmartRect::clampedZone() const {
    if(clamps == ict::L_CLAMPED) return ict::L_ZONE;
    if(clamps == ict::R_CLAMPED) return ict::R_ZONE;
    if(clamps == ict::T_CLAMPED) return ict::T_ZONE;
    if(clamps == ict::B_CLAMPED) return ict::B_ZONE;
    if(clamps == ict::RT_CLAMPED) return ict::RT_ZONE;
    if(clamps == ict::LT_CLAMPED) return ict::LT_ZONE;
    if(clamps == ict::RB_CLAMPED) return ict::RB_ZONE;
    if(clamps == ict::LB_CLAMPED) return ict::LB_ZONE;
    return ict::NONE_ZONE;
}

void SmartRect::checkMinimum() {
    wxDouble minx, miny;
    switch(activeZone) {
        case ict::R_ZONE:
        case ict::RT_ZONE:
        case ict::RB_ZONE:
            if(!useInflate()) minx = mark.GetLeft();
            else minx = mark.GetCentre().m_x;
            if(GetRight() > minx - ict::MINUPP &&
                GetRight() < minx + ict::MINUPP)
                SetRight(minx + ict::MINUPP);
            break;
        case ict::L_ZONE:
        case ict::LT_ZONE:
        case ict::LB_ZONE:
            if(!useInflate()) minx = mark.GetRight();
            else minx = mark.GetCentre().m_x;
            if(GetLeft() < minx + ict::MINUPP &&
                GetLeft() > minx - ict::MINUPP)
                SetLeft(minx - ict::MINUPP);
            break;
    }
    switch(activeZone) {
        case ict::T_ZONE:
        case ict::LT_ZONE:
        case ict::RT_ZONE:
            if(!useInflate()) miny = GetBottom();
            else miny = mark.GetCentre().m_y;
            if(GetTop() < miny + ict::MINUPP &&
                GetTop() > miny - ict::MINUPP)
                SetTop(miny - ict::MINUPP);
            break;
        case ict::B_ZONE:
        case ict::LB_ZONE:
        case ict::RB_ZONE:
            if(!useInflate()) miny = GetTop();
            else miny = mark.GetCentre().m_y;
            if(GetBottom() > miny - ict::MINUPP &&
                GetBottom() < miny + ict::MINUPP)
                SetBottom(miny + ict::MINUPP);
            break;
    }
}

void SmartRect::checkInflate() {
    if(!useInflate()) return;
    balance(activeZone);
}

void SmartRect::balance(int zone) {
    wxPoint2DDouble mcen(mark.GetCentre());
    switch(zone) {
        case ict::R_ZONE:
        case ict::RT_ZONE:
        case ict::RB_ZONE:
            SetLeft(mcen.m_x - (GetRight() - mcen.m_x));
            break;
        case ict::L_ZONE:
        case ict::LT_ZONE:
        case ict::LB_ZONE:
            SetRight(mcen.m_x + (mcen.m_x - GetLeft()));
            break;
    }
    switch(zone) {
        case ict::T_ZONE:
        case ict::RT_ZONE:
        case ict::LT_ZONE:
            SetBottom(mcen.m_y + (mcen.m_y - GetTop()));
            break;
        case ict::B_ZONE:
        case ict::RB_ZONE:
        case ict::LB_ZONE:
            SetTop(mcen.m_y - (GetBottom() - mcen.m_y));
            break;
    }
}

void SmartRect::checkReflection() {
    if(m_width < 0) {
        reflection ^= ict::HORI_REFLEC;
        activeZone ^= ict::HORI_REFLEC;
        m_x += m_width;
        m_width = abs(m_width);
    }
    if(m_height < 0) {
        reflection ^= ict::VERT_REFLEC;
        activeZone ^= ict::VERT_REFLEC;
        m_y += m_height;
        m_height = abs(m_height);
    }
}

void SmartRect::checkAspectRatio(int want) {
    if(!fixedAspectRatio() || !resizing()) return;
    wxDouble nw = m_width, nh = m_height;
    if(m_width / m_height != aspectRatio) {
        if(cornerActivated() || want == SMALLER_RECT) {
            if(want == SMALLER_RECT) {
                if(m_width / m_height < aspectRatio) nh = calcOrdi(nw);
                else nw = calcAbsc(nh);
            } else {
                if(m_width / m_height < aspectRatio) nw = calcAbsc(nh);
                else nh = calcOrdi(nw);
            }
        } else { // edgeActivated()
            if(activeZone == ict::T_ZONE || activeZone == ict::B_ZONE)
                nw = calcAbsc(nh);
            else nh = calcOrdi(nw);
        }
    }
    if(useInflate()) {
        m_width = nw;
        m_height = nh;
        SetCentre(mark.GetCentre());
        return;
    }
    switch(activeZone) {
        case ict::R_ZONE:
        case ict::RT_ZONE:
        case ict::RB_ZONE:
            m_width = nw;
            break;
        case ict::L_ZONE:
        case ict::LT_ZONE:
        case ict::LB_ZONE:
            SetLeft(GetRight() - nw);
            break;
    }
    switch(activeZone) {
        case ict::T_ZONE:
        case ict::LT_ZONE:
        case ict::RT_ZONE:
            SetTop(GetBottom() - nh);
            break;
        case ict::B_ZONE:
        case ict::LB_ZONE:
        case ict::RB_ZONE:
            m_height = nh;
            break;
    }
    if(!edgeActivated()) return;

    // side effects of the previous switches,
    // due to the use of fixed aspect ratio and edges (not corners)
    wxPoint2DDouble ncen(GetCentre());
    switch(activeZone) {
        case ict::R_ZONE:
        case ict::L_ZONE:
            ncen.m_y = mark.GetCentre().m_y;
            m_height = nh;
            break;
        case ict::T_ZONE:
        case ict::B_ZONE:
            ncen.m_x = mark.GetCentre().m_x;
            m_width = nw;
            break;
    }
    SetCentre(ncen);
}

void SmartRect::checkRestriction() {
    if(!restricted || restrictionLimits().Contains(*this)) return;
    checkLimits(true);
    if(resizing()) checkAspectRatio(SMALLER_RECT);
}

wxDouble SmartRect::calcAbsc(const wxDouble &ordi) const {
    return ordi * aspectRatio;
}

wxDouble SmartRect::calcOrdi(const wxDouble &absc) const {
    return absc / aspectRatio;
}

void SmartRect::restrict(const bool r) {
    restricted = r;
    if(restricted) setGeometry(*this);
}

void SmartRect::setRestriction(const wxRect2DDouble &r) {
    restriction = r;
    return restrict(restricted);
}

void SmartRect::setMark() {
    mark = *this;
    markZone = activeZone;
}

void SmartRect::useMark() {
    wxRect2DDouble::operator=(mark);
    activeZone = markZone;
    setAspectRatio(m_width / m_height);
}

void SmartRect::fixedAspectRatio(const bool ar) {
    fixed = ar;
}

void SmartRect::setAspectRatio(const wxDouble &ar) {
    aspectRatio = ar;
}

void SmartRect::setAspectRatio(const int x, const int y) {
    aspectRatio = (wxDouble)x / y;
}

void SmartRect::useInflate(const bool ec) {
    inflate = ec;
}

bool SmartRect::fixedAspectRatio() const {
    return fixed;
}

bool SmartRect::useInflate() const {
    return inflate;
}

bool SmartRect::isRestricted() const {
    return restricted;
}

SmartRect::~SmartRect() {

}
