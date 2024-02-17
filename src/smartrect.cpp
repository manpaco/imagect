#include "smartrect.hpp"
#include "recthelper.hpp"
#include "defs.hpp"
#include <iostream>
#include <iomanip>

SmartRect::SmartRect() : SmartRect(wxRect2DDouble(0, 0, 1, 1)) {

}

SmartRect::SmartRect(const wxRect2DDouble &r) {
    *this = r;
    this->fixed = false;
    this->restricted = false;
    this->centered = false;
    this->grid = false;
    this->accLeft = 0;
    this->accTop = 0;
    this->accWidth = 0;
    this->accHeight = 0;
    saveInstant();
    setMark();
}

bool SmartRect::setGeometry(const wxRect2DDouble &r) {
    *this = r;
    if(isRestricted()) {
        if(isCentered() && !restriction.Contains(GetCentre()))
            SetCentre(restriction.GetCentre());
        else if(!isCentered() && !restriction.Contains(GetPosition())) {
            modifyPosition(restriction.m_x, restriction.m_y);
        }
    }
    return pushZoneTo(ict::RB_ZONE, GetRightBottom());
}

bool SmartRect::setPosition(const wxPoint2DDouble &r) {
    return setGeometry(wxRect2DDouble(r.m_x, r.m_y, m_width, m_height));
}

bool SmartRect::setSize(const wxPoint2DDouble &r) {
    return setGeometry(wxRect2DDouble(m_x, m_y, r.m_x, r.m_y));
}

void SmartRect::modifyGeometry(wxDouble x, wxDouble y, wxDouble w, wxDouble h) {
    modifyPosition(x, y);
    modifySize(w, h);
}

void SmartRect::modifyPosition(wxDouble x, wxDouble y) {
    m_x = x; m_y = y;
}

void SmartRect::modifySize(wxDouble w, wxDouble h) {
    m_width = w; m_height = h;
}

wxDouble SmartRect::internalGetLeft() const {
    return GetLeft() + accLeft;
}

wxDouble SmartRect::internalGetTop() const {
    return GetTop() + accTop;
}

wxDouble SmartRect::internalGetRight() const {
    return GetRight() + accWidth;
}

wxDouble SmartRect::internalGetBottom() const {
    return GetBottom() + accHeight;
}

void SmartRect::placeInPlayground(wxPoint2DDouble *p, bool inner) const {
    wxRect2DDouble playground;
    if(inner) playground = getInnerPlayground();
    else playground = getPlayground();
    if(playground.Contains(*p)) return;
    if(p->m_x < playground.GetLeft()) p->m_x = playground.GetLeft();
    if(p->m_x > playground.GetRight()) p->m_x = playground.GetRight();
    if(p->m_y < playground.GetTop()) p->m_y = playground.GetTop();
    if(p->m_y > playground.GetBottom()) p->m_y = playground.GetBottom();
}

void SmartRect::useGrid(bool op) {
    if(grid != op) {
        grid = op;
        if(useGrid()) assembleGrid();
        else disassembleGrid();
    }
}

void SmartRect::clearAccums() {
    accLeft = accTop = accWidth = accHeight = 0;
}

void SmartRect::disassembleGrid() {
    m_width = internalGetRight() - internalGetLeft();
    m_height = internalGetBottom() - internalGetTop();
    m_x += accLeft;
    m_y += accTop;
}

void SmartRect::assembleGrid() {
    clearAccums();
    wxDouble re(GetRight());
    wxDouble be(GetBottom());
    m_x = accumulateLeft(m_x);
    m_y = accumulateTop(m_y);
    m_width = accumulateWidth(re) - m_x;
    m_height = accumulateHeight(be) - m_y;
    if(isCentered()) {
        balanceAbscAccums();
        balanceOrdiAccums();
    }
}

bool SmartRect::useGrid() const {
    return grid;
}

wxRect2DDouble SmartRect::getInnerPlayground() const {
    return wxRect2DDouble(restriction.m_x, restriction.m_y, restriction.m_width - m_width, restriction.m_height - m_height);
}

wxRect2DDouble SmartRect::getPlayground() const {
    if(isCentered()) {
        wxRect2DDouble playground;
        wxPoint2DDouble cen(GetCentre());
        wxDouble minWidth = restriction.GetRight() - cen.m_x;
        if(minWidth > cen.m_x - restriction.GetLeft()) minWidth = cen.m_x - restriction.GetLeft();
        wxDouble minHeight = restriction.GetBottom() - cen.m_y;
        if(minHeight > cen.m_y - restriction.GetTop()) minHeight = cen.m_y - restriction.GetTop();
        playground.m_x = cen.m_x - minWidth;
        playground.m_y = cen.m_y - minHeight;
        playground.m_width = minWidth * 2;
        playground.m_height = minHeight * 2;
        return playground;
    } else {
        return restriction;
    }
}

wxDouble SmartRect::getAspectRatio() const {
    return aspectRatio;
}

int SmartRect::getReflection() const {
    return reflection;
}

ict::RectZone SmartRect::getLastZone() const {
    return lastZone;
}

wxRect2DDouble SmartRect::getChangeUnion() const {
    return instant.CreateUnion(*this);
}

bool SmartRect::pushZoneTo(ict::RectZone z, const wxPoint2DDouble &p) {
    wxPoint2DDouble inp(p);
    if(!reflecting) {
        saveInstant();
        if(isRestricted()) placeInPlayground(&inp, z == ict::IN_ZONE);
    }
    int lastReflection = ict::NONE_REFLEC;
    lastZone = z;
    bool changed = false;
    switch (z) {
        case ict::RB_ZONE: {
            wxPoint2DDouble zoneLimit(GetLeftTop());
            if(isCentered()) zoneLimit = GetCentre();
            if(inp.m_x < zoneLimit.m_x - ict::MINUPP) {
                lastReflection ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x - m_width;
                reflecting = true;
            } else if(inp.m_x < zoneLimit.m_x + ict::MINUPP) {
                inp.m_x = zoneLimit.m_x + ict::MINUPP;
            }
            if(inp.m_y < zoneLimit.m_y - ict::MINUPP) {
                lastReflection ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y - m_height;
                reflecting = true;
            } else if(inp.m_y < zoneLimit.m_y + ict::MINUPP) {
                inp.m_y = zoneLimit.m_y + ict::MINUPP;
            }
            reflection ^= lastReflection;
            if(lastReflection == ict::HORI_REFLEC) changed = pushZoneTo(ict::LB_ZONE, inp);
            else if(lastReflection == ict::VERT_REFLEC) changed = pushZoneTo(ict::RT_ZONE, inp);
            else if(lastReflection == ict::HOVE_REFLEC) changed = pushZoneTo(ict::LT_ZONE, inp);
            else changed = pushRightBottomTo(inp);
            break;
        }
        case ict::LT_ZONE: {
            wxPoint2DDouble zoneLimit(GetRightBottom());
            if(isCentered()) zoneLimit = GetCentre();
            if(inp.m_x > zoneLimit.m_x + ict::MINUPP) {
                lastReflection ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x + m_width;
                reflecting = true;
            } else if(inp.m_x > zoneLimit.m_x - ict::MINUPP) {
                inp.m_x = zoneLimit.m_x - ict::MINUPP;
            }
            if(inp.m_y > zoneLimit.m_y + ict::MINUPP) {
                lastReflection ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y + m_height;
                reflecting = true;
            } else if(inp.m_y > zoneLimit.m_y - ict::MINUPP) {
                inp.m_y = zoneLimit.m_y - ict::MINUPP;
            }
            reflection ^= lastReflection;
            if(lastReflection == ict::HORI_REFLEC) changed = pushZoneTo(ict::RT_ZONE, inp);
            else if(lastReflection == ict::VERT_REFLEC) changed = pushZoneTo(ict::LB_ZONE, inp);
            else if(lastReflection == ict::HOVE_REFLEC) changed = pushZoneTo(ict::RB_ZONE, inp);
            else changed = pushLeftTopTo(inp);
            break;
        }
        case ict::RT_ZONE: {
            wxPoint2DDouble zoneLimit(GetLeftBottom());
            if(isCentered()) zoneLimit = GetCentre();
            if(inp.m_x < zoneLimit.m_x - ict::MINUPP) {
                lastReflection ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x - m_width;
                reflecting = true;
            } else if(inp.m_x < zoneLimit.m_x + ict::MINUPP) {
                inp.m_x = zoneLimit.m_x + ict::MINUPP;
            }
            if(inp.m_y > zoneLimit.m_y + ict::MINUPP) {
                lastReflection ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y + m_height;
                reflecting = true;
            } else if(inp.m_y > zoneLimit.m_y - ict::MINUPP) {
                inp.m_y = zoneLimit.m_y - ict::MINUPP;
            }
            reflection ^= lastReflection;
            if(lastReflection == ict::HORI_REFLEC) changed = pushZoneTo(ict::LT_ZONE, inp);
            else if(lastReflection == ict::VERT_REFLEC) changed = pushZoneTo(ict::RB_ZONE, inp);
            else if(lastReflection == ict::HOVE_REFLEC) changed = pushZoneTo(ict::LB_ZONE, inp);
            else changed = pushRightTopTo(inp);
            break;
        }
        case ict::LB_ZONE: {
            wxPoint2DDouble zoneLimit(GetRightTop());
            if(isCentered()) zoneLimit = GetCentre();
            if(inp.m_x > zoneLimit.m_x + ict::MINUPP) {
                lastReflection ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x + m_width;
                reflecting = true;
            } else if(inp.m_x > zoneLimit.m_x - ict::MINUPP) {
                inp.m_x = zoneLimit.m_x - ict::MINUPP;
            }
            if(inp.m_y < zoneLimit.m_y - ict::MINUPP) {
                lastReflection ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y - m_height;
                reflecting = true;
            } else if(inp.m_y < zoneLimit.m_y + ict::MINUPP) {
                inp.m_y = zoneLimit.m_y + ict::MINUPP;
            }
            reflection ^= lastReflection;
            if(lastReflection == ict::HORI_REFLEC) changed = pushZoneTo(ict::RB_ZONE, inp);
            else if(lastReflection == ict::VERT_REFLEC) changed = pushZoneTo(ict::LT_ZONE, inp);
            else if(lastReflection == ict::HOVE_REFLEC) changed = pushZoneTo(ict::RT_ZONE, inp);
            else changed = pushLeftBottomTo(inp);
            break;
        }
        case ict::T_ZONE: {
            wxDouble zoneLimit(GetBottom());
            if(isCentered()) zoneLimit = GetCentre().m_y;
            if(inp.m_y > zoneLimit + ict::MINUPP) {
                lastReflection ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y + m_height;
                reflecting = true;
            } else if(inp.m_y > zoneLimit - ict::MINUPP) {
                inp.m_y = zoneLimit - ict::MINUPP;
            }
            reflection ^= lastReflection;
            if(lastReflection == ict::VERT_REFLEC) changed = pushZoneTo(ict::B_ZONE, inp);
            else changed = pushTopTo(inp.m_y);
            break;
        }
        case ict::B_ZONE: {
            wxDouble zoneLimit(GetTop());
            if(isCentered()) zoneLimit = GetCentre().m_y;
            if(inp.m_y < zoneLimit - ict::MINUPP) {
                lastReflection ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y - m_height;
                reflecting = true;
            } else if(inp.m_y < zoneLimit + ict::MINUPP) {
                inp.m_y = zoneLimit + ict::MINUPP;
            }
            reflection ^= lastReflection;
            if(lastReflection == ict::VERT_REFLEC) changed = pushZoneTo(ict::T_ZONE, inp);
            else changed = pushBottomTo(inp.m_y);
            break;
        }
        case ict::L_ZONE: {
            wxDouble zoneLimit(GetRight());
            if(isCentered()) zoneLimit = GetCentre().m_x;
            if(inp.m_x > zoneLimit + ict::MINUPP) {
                lastReflection ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x + m_width;
                reflecting = true;
            } else if(inp.m_x > zoneLimit - ict::MINUPP) {
                inp.m_x = zoneLimit - ict::MINUPP;
            }
            reflection ^= lastReflection;
            if(lastReflection == ict::HORI_REFLEC) changed = pushZoneTo(ict::R_ZONE, inp);
            else changed = pushLeftTo(inp.m_x);
            break;
        }
        case ict::R_ZONE: {
            wxDouble zoneLimit(GetLeft());
            if(isCentered()) zoneLimit = GetCentre().m_x;
            if(inp.m_x < zoneLimit - ict::MINUPP) {
                lastReflection ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x - m_width;
                reflecting = true;
            } else if(inp.m_x < zoneLimit + ict::MINUPP) {
                inp.m_x = zoneLimit + ict::MINUPP;
            }
            reflection ^= lastReflection;
            if(lastReflection == ict::HORI_REFLEC) changed = pushZoneTo(ict::L_ZONE, inp);
            else changed = pushRightTo(inp.m_x);
            break;
        }
        case ict::IN_ZONE: {
            changed = pushTo(inp);
            break;
        }
        default: {
            changed = false;
            break;
        }
    }
    reflecting = false;
    return changed;
}

bool SmartRect::pushTo(const wxPoint2DDouble &p) {
    if(p == GetPosition()) return false;
    wxPoint2DDouble inp(p);
    if(useGrid()) {
        inp.m_x = round(inp.m_x);
        inp.m_y = round(inp.m_y);
    }
    modifyPosition(inp.m_x, inp.m_y);
    return instantChanged();
}

bool SmartRect::pushRightBottomTo(const wxPoint2DDouble &p) {
    wxDouble dx = p.m_x - (internalGetRight());
    wxDouble dy = p.m_y - (internalGetBottom());
    if(isFixed()) {
        if(p.m_x >= internalGetRight() && p.m_y <= internalGetBottom()) {
            dx = calcAbsc(dy);
        } else if(p.m_x <= internalGetRight() && p.m_y >= internalGetBottom()) {
            dy = calcOrdi(dx);
        } else if(p.m_x < internalGetRight() && p.m_y < internalGetBottom()) {
            wxDouble proy = dx / dy;
            if(proy < aspectRatio) dx = calcAbsc(dy);
            else dy = calcOrdi(dx);
        } else if(p.m_x > internalGetRight() && p.m_y > internalGetBottom()) {
            wxDouble proy = dx / dy;
            if(proy > aspectRatio) dx = calcAbsc(dy);
            else dy = calcOrdi(dx);
        }
    }
    if(useGrid()) {
        wxDouble rdx, bdy;
        rdx = accumulateWidth(dx);
        bdy = accumulateHeight(dy);
        m_width += rdx;
        m_height += bdy;
        if(isCentered()) {
            wxDouble ldx, tdy;
            ldx = accumulateLeft(-dx);
            tdy = accumulateTop(-dy);
            m_x -= ldx; m_width += ldx + rdx;
            m_y -= tdy; m_height += tdy + bdy;
        }
    } else {
        m_width += dx;
        m_height += dy;
        if(isCentered()) {
            m_x -= dx; m_width += dx;
            m_y -= dy; m_height += dy;
        }
    }
    return instantChanged();
}

bool SmartRect::pushLeftTopTo(const wxPoint2DDouble &p) {
    wxDouble deltaX = p.m_x - GetLeft();
    wxDouble deltaY = p.m_y - GetTop();
    if(isFixed()) {
        if(p.m_x >= GetLeft() && p.m_y <= GetTop()) {
            deltaY = calcOrdi(deltaX);
        } else if(p.m_x <= GetLeft() && p.m_y >= GetTop()) {
            deltaX = calcAbsc(deltaY);
        } else if(p.m_x < GetLeft() && p.m_y < GetTop()) {
            wxDouble proy = deltaX / deltaY;
            if(proy > aspectRatio) deltaX = calcAbsc(deltaY);
            else deltaY = calcOrdi(deltaX);
        } else if(p.m_x > GetLeft() && p.m_y > GetTop()) {
            wxDouble proy = deltaX / deltaY;
            if(proy < aspectRatio) deltaX = calcAbsc(deltaY);
            else deltaY = calcOrdi(deltaX);
        }
    }
    m_x += deltaX;
    m_y += deltaY;
    m_width -= deltaX;
    m_height -= deltaY;
    if(isCentered()) {
        m_width -= deltaX;
        m_height -= deltaY;
    }
    return instantChanged();
}

bool SmartRect::pushRightTopTo(const wxPoint2DDouble &p) {
    wxDouble deltaX = p.m_x - GetRight();
    wxDouble deltaY = p.m_y - GetTop();
    if(isFixed()) {
        if(p.m_x <= GetRight() && p.m_y <= GetTop()) {
            deltaY = calcOrdi(-deltaX);
        } else if(p.m_x >= GetRight() && p.m_y >= GetTop()) {
            deltaX = calcAbsc(-deltaY);
        } else if(p.m_x > GetRight() && p.m_y < GetTop()) {
            wxDouble proy = deltaX / -deltaY;
            if(proy > aspectRatio) deltaX = calcAbsc(-deltaY);
            else deltaY = calcOrdi(-deltaX);
        } else if(p.m_x < GetRight() && p.m_y > GetTop()) {
            wxDouble proy = deltaX / -deltaY;
            if(proy < aspectRatio) deltaX = calcAbsc(-deltaY);
            else deltaY = calcOrdi(-deltaX);
        }
    }
    m_y += deltaY;
    m_width += deltaX;
    m_height -= deltaY;
    if(isCentered()) {
        m_x -= deltaX;
        m_width += deltaX;
        m_height -= deltaY;
    }
    return instantChanged();
}

bool SmartRect::pushLeftBottomTo(const wxPoint2DDouble &p) {
    wxDouble deltaX = p.m_x - GetLeft();
    wxDouble deltaY = p.m_y - GetBottom();
    if(isFixed()) {
        if(p.m_x <= GetLeft() && p.m_y <= GetBottom()) {
            deltaX = calcAbsc(-deltaY);
        } else if(p.m_x >= GetLeft() && p.m_y >= GetBottom()) {
            deltaY = calcOrdi(-deltaX);
        } else if(p.m_x > GetLeft() && p.m_y < GetBottom()) {
            wxDouble proy = deltaX / -deltaY;
            if(proy < aspectRatio) deltaX = calcAbsc(-deltaY);
            else deltaY = calcOrdi(-deltaX);
        } else if(p.m_x < GetLeft() && p.m_y > GetBottom()) {
            wxDouble proy = deltaX / -deltaY;
            if(proy > aspectRatio) deltaX = calcAbsc(-deltaY);
            else deltaY = calcOrdi(-deltaX);
        }
    }
    m_x += deltaX;
    m_width -= deltaX;
    m_height += deltaY;
    if(isCentered()) {
        m_width -= deltaX;
        m_y -= deltaY;
        m_height += deltaY;
    }
    return instantChanged();
}

bool SmartRect::pushTopTo(const wxDouble &p) {
    wxDouble deltaX = 0;
    wxDouble deltaY = p - GetTop();
    if(isFixed()) deltaX = calcAbsc(deltaY);
    m_x += deltaX / 2;
    m_width -= deltaX;
    m_y += deltaY;
    m_height -= deltaY;
    if(isCentered()) {
        m_x += deltaX / 2;
        m_width -= deltaX;
        m_height -= deltaY;
    }
    return instantChanged();
}

bool SmartRect::pushBottomTo(const wxDouble &p) {
    wxDouble deltaX = 0;
    wxDouble deltaY = p - GetBottom();
    if(isFixed()) deltaX = calcAbsc(-deltaY);
    m_x += deltaX / 2;
    m_width -= deltaX;
    m_height += deltaY;
    if(isCentered()) {
        m_x += deltaX / 2;
        m_width -= deltaX;
        m_y -= deltaY;
        m_height += deltaY;
    }
    return instantChanged();
}

bool SmartRect::pushLeftTo(const wxDouble &p) {
    wxDouble deltaX = p - GetLeft();
    wxDouble deltaY = 0;
    if(isFixed()) deltaY = calcOrdi(deltaX);
    m_x += deltaX;
    m_y += deltaY / 2;
    m_width -= deltaX;
    m_height -= deltaY;
    if(isCentered()) {
        m_width -= deltaX;
        m_y += deltaY / 2;
        m_height -= deltaY;
    }
    return instantChanged();
}

bool SmartRect::pushRightTo(const wxDouble &p) {
    wxDouble dx = p - internalGetRight();
    wxDouble dy = 0;
    if(isFixed()) {
        dy = calcOrdi(dx) / 2;
        if(useGrid() && !isCentered()) balanceOrdiAccums();
    }
    if(useGrid()) {
        wxDouble rdx, tdy, bdy;
        rdx = accumulateWidth(dx);
        m_width += rdx;
        bdy = accumulateHeight(dy);
        tdy = accumulateTop(-dy);
        m_y += tdy;
        m_height += bdy - tdy;
        if(isCentered()) {
            wxDouble ldx;
            ldx = accumulateLeft(-dx);
            m_x += ldx;
            m_width -= ldx;
            bdy = accumulateHeight(dy);
            tdy = accumulateTop(-dy);
            m_y += tdy;
            m_height += bdy - tdy;
        }
    } else {
        m_width += dx;
        m_y -= dy;
        m_height += dy * 2;
        if(isCentered()) {
            m_x -= dx;
            m_width += dx;
            m_y -= dy;
            m_height += dy * 2;
        }
    }
    wxDouble iw, ih;
    return instantChanged();
}

wxDouble SmartRect::accumulateLeft(const wxDouble &dl) {
    wxDouble ret;
    accLeft += dl;
    ret = floor(accLeft);
    accLeft -= ret;
    return ret;
}

wxDouble SmartRect::accumulateTop(const wxDouble &dt) {
    wxDouble ret;
    accTop += dt;
    ret = floor(accTop);
    accTop -= ret;
    return ret;
}

wxDouble SmartRect::accumulateWidth(const wxDouble &dr) {
    wxDouble ret;
    accWidth += dr;
    ret = ceil(accWidth);
    accWidth -= ret;
    return ret;
}

wxDouble SmartRect::accumulateHeight(const wxDouble &db) {
    wxDouble ret;
    accHeight += db;
    ret = ceil(accHeight);
    accHeight -= ret;
    return ret;
}

void SmartRect::balanceOrdiAccums() {
    wxDouble avg(accTop + accHeight);
    if(avg == 0) return;
    avg /= 2;
    accTop -= avg;
    accHeight -= avg;
}

void SmartRect::balanceAbscAccums() {
    wxDouble avg(accLeft + accWidth);
    avg /= 2;
    accLeft -= avg;
    accWidth -= avg;
}

wxRect2DDouble SmartRect::inflatedRect(const wxDouble &x, const wxDouble &y) const {
    return wxRect2DDouble(GetLeft() - x, GetTop() - y, GetRight() + (2 * x), GetBottom() + (2 * y));
}

wxDouble SmartRect::calcAbsc(const wxDouble &ordi) const {
    return ordi * aspectRatio;
}

wxDouble SmartRect::calcOrdi(const wxDouble &absc) const {
    return absc / aspectRatio;
}

bool SmartRect::restrict(const bool r) {
    restricted = r;
    if(restricted) return setGeometry(*this);
    else return false;
}

bool SmartRect::setRestriction(const wxRect2DDouble &r) {
    restriction = r;
    return restrict(restricted);
}

void SmartRect::setMark() {
    mark = *this;
}

void SmartRect::useMark() {
    *this = mark;
}

void SmartRect::fixedAspectRatio(const bool ar) {
    fixed = ar;
    if(fixed) setAspectRatio(m_width, m_height);
}

void SmartRect::setAspectRatio(const wxDouble &ar) {
    aspectRatio = ar;
}

void SmartRect::setAspectRatio(const int x, const int y) {
    aspectRatio = (wxDouble)x / y;
}

void SmartRect::expandFromCenter(const bool ec) {
    centered = ec;
    if(centered) {
        balanceAbscAccums();
        balanceOrdiAccums();
    }
}

bool SmartRect::isFixed() const {
    return fixed;
}

bool SmartRect::isCentered() const {
    return centered;
}

bool SmartRect::isRestricted() const {
    return restricted;
}

void SmartRect::saveInstant() {
    instant = *this;
}

bool SmartRect::instantChanged(bool saveNew) {
    if (instant != *this) return true;
    else return false;
}

SmartRect &SmartRect::operator=(const wxRect2DDouble &r) {
    modifyGeometry(r.m_x, r.m_y, r.m_width, r.m_height);
    return *this;
}

SmartRect::~SmartRect() {

}
