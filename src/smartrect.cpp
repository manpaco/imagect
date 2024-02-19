#include "smartrect.hpp"
#include "recthelper.hpp"
#include "defs.hpp"
#include <iostream>
#include <iomanip>

SmartRect::SmartRect() : SmartRect(wxRect2DDouble(0, 0, 1, 1)) {

}

SmartRect::SmartRect(const wxRect2DDouble &r) {
    wxRect2DDouble::operator=(r);
    fixed = false;
    restricted = false;
    centered = false;
    grid = false;
    save = true;
    reflection = ict::NONE_REFLEC;
    lastZone = ict::NONE_ZONE;
    saveInstant();
    setMark();
    clearDummySpace();
}

void SmartRect::saveBefore(bool c) {
    save = c;
}

bool SmartRect::restrictionContains(const wxPoint2DDouble &p) const {
    return p.m_x >= rx && p.m_x < rx + rw &
           p.m_y >= ry && p.m_y < ry + rh;
}
bool SmartRect::setGeometry(const wxRect2DDouble &r) {
    wxRect2DDouble newg(r);
    saveInstant();
    m_x = newg.m_x;
    m_y = newg.m_y;
    newg.m_width = abs(newg.m_width);
    newg.m_height = abs(newg.m_height);
    if(isRestricted() && !restrictionContains(GetPosition())) {
        m_x = rx;
        m_y = ry;
    }
    bool aux = centered;
    if(aux) centered = !aux;
    saveBefore(false);
    pushZoneTo(ict::RB_ZONE, wxPoint2DDouble(m_x + newg.m_width,
                                             m_y + newg.m_height));
    saveBefore(true);
    centered = aux;
    return instantChanged();
}

bool SmartRect::setPosition(const wxPoint2DDouble &p) {
    return pushZoneTo(ict::IN_ZONE, p);
}

bool SmartRect::setSize(const wxPoint2DDouble &s) {
    wxPoint2DDouble newp(s);
    newp.m_x = abs(newp.m_x);
    newp.m_y = abs(newp.m_y);
    if(isCentered()) {
        newp.m_x /= 2;
        newp.m_y /= 2;
        newp += GetCentre();
    } else newp += GetPosition();
    return pushZoneTo(ict::RB_ZONE, newp);
}

wxDouble SmartRect::extGetLeft() const {
    if(useGrid()) {
        if(m_x < 0) return round_htz(m_x);
        else return round_haz(m_x);
    }
    else return m_x;
}

wxDouble SmartRect::extGetTop() const {
    if(useGrid()) {
        if(m_y < 0) return round_htz(m_y);
        else return round_haz(m_y);
    }
    else return m_y;
}

wxDouble SmartRect::extGetRight() const {
    if(useGrid()) {
        if(m_x + m_width < 0) return round_haz(m_x + m_width);
        else return round_htz(m_x + m_width);
    }
    else return m_x + m_width;
}

wxDouble SmartRect::extGetBottom() const {
    if(useGrid()) {
        if(m_y + m_height < 0) return round_haz(m_y + m_height);
        else return round_htz(m_y + m_height);
    }
    else return m_y + m_height;
}

wxDouble SmartRect::extGetWidth() const {
    if(useGrid()) return extGetRight() - extGetLeft();
    else return m_width;
}

wxDouble SmartRect::extGetHeight() const {
    if(useGrid()) return extGetBottom() - extGetTop();
    else return m_height;
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
    grid = op;
}

bool SmartRect::useGrid() const {
    return grid;
}

wxRect2DDouble SmartRect::getInnerPlayground() const {
/*     return wxRect2DDouble(restriction.m_x, restriction.m_y, restriction.m_width - m_width, restriction.m_height - m_height); */
}

wxRect2DDouble SmartRect::getPlayground() const {
    /* if(isCentered()) {
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
    } */
}

wxDouble SmartRect::getAspectRatio() const {
    return aspectRatio;
}

int SmartRect::getReflection() const {
    return reflection;
}

int SmartRect::getLastZone() const {
    return lastZone;
}

void SmartRect::setDummySpace(const wxDouble &dsx, const wxDouble &dsy) {
    dummyX = abs(dsx);
    dummyY = abs(dsy);
}

void SmartRect::clearDummySpace() {
    dummyX = dummyY = 0;
}
wxRect2DDouble SmartRect::getChangeUnion() const {
    wxRect2DDouble ir(ix, iy, iw, ih);
    wxRect2DDouble gr(extGetLeft(), extGetTop(),
                      extGetWidth(), extGetHeight());
    return ir.CreateUnion(gr);
}

bool SmartRect::pushZoneTo(const int z, const wxPoint2DDouble &p) {
    if(save) saveInstant();
    lastZone = z;
    wxPoint2DDouble inp(p);
    wxDouble limitx, limity;
    if(isCentered()) {
        wxPoint2DDouble icen(GetCentre());
        limitx = icen.m_x;
        limity = icen.m_y;
    }
    switch (lastZone) {
        case ict::RB_ZONE:
            if(!isCentered()) {
                limitx = GetLeft();
                limity = GetTop();
            }
            if(inp.m_x < limitx - ict::MINUPP - dummyX) {
                inp.m_x += dummyX;
                reflection ^= lastZone ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x - m_width;
            } else if(inp.m_x < limitx + ict::MINUPP) {
                inp.m_x = limitx + ict::MINUPP;
            }
            if(inp.m_y < limity - ict::MINUPP - dummyY) {
                inp.m_y += dummyY;
                reflection ^= lastZone ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y - m_height;
            } else if(inp.m_y < limity + ict::MINUPP) {
                inp.m_y = limity + ict::MINUPP;
            }
            break;
        case ict::LT_ZONE:
            if(!isCentered()) {
                limitx = GetRight();
                limity = GetBottom();
            }
            if(inp.m_x > limitx + ict::MINUPP + dummyX) {
                inp.m_x -= dummyX;
                reflection ^= lastZone ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x + m_width;
            } else if(inp.m_x > limitx - ict::MINUPP) {
                inp.m_x = limitx - ict::MINUPP;
            }
            if(inp.m_y > limity + ict::MINUPP + dummyY) {
                inp.m_y -= dummyY;
                reflection ^= lastZone ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y + m_height;
            } else if(inp.m_y > limity - ict::MINUPP) {
                inp.m_y = limity - ict::MINUPP;
            }
            break;
        case ict::RT_ZONE:
            if(!isCentered()) {
                limitx = GetLeft();
                limity = GetBottom();
            }
            if(inp.m_x < limitx - ict::MINUPP - dummyX) {
                inp.m_x += dummyX;
                reflection ^= lastZone ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x - m_width;
            } else if(inp.m_x < limitx + ict::MINUPP) {
                inp.m_x = limitx + ict::MINUPP;
            }
            if(inp.m_y > limity + ict::MINUPP + dummyY) {
                inp.m_y -= dummyY;
                reflection ^= lastZone ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y + m_height;
            } else if(inp.m_y > limity - ict::MINUPP) {
                inp.m_y = limity - ict::MINUPP;
            }
            break;
        case ict::LB_ZONE:
            if(!isCentered()) {
                limitx = GetRight();
                limity = GetTop();
            }
            if(inp.m_x > limitx + ict::MINUPP + dummyX) {
                inp.m_x -= dummyX;
                reflection ^= lastZone ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x + m_width;
            } else if(inp.m_x > limitx - ict::MINUPP) {
                inp.m_x = limitx - ict::MINUPP;
            }
            if(inp.m_y < limity - ict::MINUPP - dummyY) {
                inp.m_y += dummyY;
                reflection ^= lastZone ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y - m_height;
            } else if(inp.m_y < limity + ict::MINUPP) {
                inp.m_y = limity + ict::MINUPP;
            }
            break;
        case ict::T_ZONE:
            if(!isCentered()) limity = GetBottom();
            if(inp.m_y > limity + ict::MINUPP + dummyY) {
                inp.m_y -= dummyY;
                reflection ^= lastZone ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y + m_height;
            } else if(inp.m_y > limity - ict::MINUPP) {
                inp.m_y = limity - ict::MINUPP;
            }
            break;
        case ict::B_ZONE:
            if(!isCentered()) limity = GetTop();
            if(inp.m_y < limity - ict::MINUPP - dummyY) {
                inp.m_y += dummyY;
                reflection ^= lastZone ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y - m_height;
            } else if(inp.m_y < limity + ict::MINUPP) {
                inp.m_y = limity + ict::MINUPP;
            }
            break;
        case ict::L_ZONE:
            if(!isCentered()) limitx = GetRight();
            if(inp.m_x > limitx + ict::MINUPP + dummyX) {
                inp.m_x -= dummyX;
                reflection ^= lastZone ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x + m_width;
            } else if(inp.m_x > limitx - ict::MINUPP) {
                inp.m_x = limitx - ict::MINUPP;
            }
            break;
        case ict::R_ZONE:
            if(!isCentered()) limitx = GetLeft();
            if(inp.m_x < limitx - ict::MINUPP - dummyX) {
                inp.m_x += dummyX;
                reflection ^= lastZone ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x - m_width;
            } else if(inp.m_x < limitx + ict::MINUPP) {
                inp.m_x = limitx + ict::MINUPP;
            }
            break;
    }
    // if(isRestricted()) placeInPlayground(&inp, lastZone == ict::IN_ZONE);
    switch (lastZone) {
        case ict::IN_ZONE:
            pushTo(inp);
            break;
        case ict::RB_ZONE:
            pushRightBottomTo(inp);
            break;
        case ict::LT_ZONE:
            pushLeftTopTo(inp);
            break;
        case ict::RT_ZONE:
            pushRightTopTo(inp);
            break;
        case ict::LB_ZONE:
            pushLeftBottomTo(inp);
            break;
        case ict::T_ZONE:
            pushTopTo(inp.m_y);
            break;
        case ict::B_ZONE:
            pushBottomTo(inp.m_y);
            break;
        case ict::L_ZONE:
            pushLeftTo(inp.m_x);
            break;
        case ict::R_ZONE:
            pushRightTo(inp.m_x);
            break;
    }
    return instantChanged();
}

void SmartRect::pushTo(const wxPoint2DDouble &p) {
    wxPoint2DDouble inp(p);
    if(useGrid()) {
        inp.m_x = round(inp.m_x);
        inp.m_y = round(inp.m_y);
    }
    m_x = inp.m_x;
    m_y = inp.m_y;
}

void SmartRect::pushRightBottomTo(const wxPoint2DDouble &p) {
    if(p.m_x == GetRight() && p.m_y == GetBottom()) return;
    wxDouble dx = p.m_x - GetRight();
    wxDouble dy = p.m_y - GetBottom();
    if(isFixed()) {
        if(p.m_x >= GetRight() && p.m_y <= GetBottom()) {
            dy = calcOrdi(dx);
        } else if(p.m_x <= GetRight() && p.m_y >= GetBottom()) {
            dx = calcAbsc(dy);
        } else if(p.m_x < GetRight() && p.m_y < GetBottom()) {
            wxDouble proy = dx / dy;
            if(proy > aspectRatio) dx = calcAbsc(dy);
            else dy = calcOrdi(dx);
        } else if(p.m_x > GetRight() && p.m_y > GetBottom()) {
            wxDouble proy = dx / dy;
            if(proy < aspectRatio) dx = calcAbsc(dy);
            else dy = calcOrdi(dx);
        }
    }
    m_width += dx;
    m_height += dy;
    if(isCentered()) {
        m_x -= dx; m_width += dx;
        m_y -= dy; m_height += dy;
    }
}

void SmartRect::pushLeftTopTo(const wxPoint2DDouble &p) {
    if(p.m_x == GetLeft() && p.m_y == GetTop()) return;
    wxDouble dx = p.m_x - GetLeft();
    wxDouble dy = p.m_y - GetTop();
    if(isFixed()) {
        if(p.m_x >= GetLeft() && p.m_y <= GetTop()) {
            dx = calcAbsc(dy);
        } else if(p.m_x <= GetLeft() && p.m_y >= GetTop()) {
            dy = calcOrdi(dx);
        } else if(p.m_x < GetLeft() && p.m_y < GetTop()) {
            wxDouble proy = dx / dy;
            if(proy < aspectRatio) dx = calcAbsc(dy);
            else dy = calcOrdi(dx);
        } else if(p.m_x > GetLeft() && p.m_y > GetTop()) {
            wxDouble proy = dx / dy;
            if(proy > aspectRatio) dx = calcAbsc(dy);
            else dy = calcOrdi(dx);
        }
    }
    m_x += dx;
    m_y += dy;
    m_width -= dx;
    m_height -= dy;
    if(isCentered()) {
        m_width -= dx;
        m_height -= dy;
    }
}

void SmartRect::pushRightTopTo(const wxPoint2DDouble &p) {
    if(p.m_x == GetRight() && p.m_y == GetTop()) return;
    wxDouble dx = p.m_x - GetRight();
    wxDouble dy = p.m_y - GetTop();
    if(isFixed()) {
        if(p.m_x <= GetRight() && p.m_y <= GetTop()) {
            dx = calcAbsc(-dy);
        } else if(p.m_x >= GetRight() && p.m_y >= GetTop()) {
            dy = calcOrdi(-dx);
        } else if(p.m_x > GetRight() && p.m_y < GetTop()) {
            wxDouble proy = dx / -dy;
            if(proy < aspectRatio) dx = calcAbsc(-dy);
            else dy = calcOrdi(-dx);
        } else if(p.m_x < GetRight() && p.m_y > GetTop()) {
            wxDouble proy = dx / -dy;
            if(proy > aspectRatio) dx = calcAbsc(-dy);
            else dy = calcOrdi(-dx);
        }
    }
    m_y += dy;
    m_width += dx;
    m_height -= dy;
    if(isCentered()) {
        m_x -= dx;
        m_width += dx;
        m_height -= dy;
    }
}

void SmartRect::pushLeftBottomTo(const wxPoint2DDouble &p) {
    if(p.m_x == GetLeft() && p.m_y == GetBottom()) return;
    wxDouble dx = p.m_x - GetLeft();
    wxDouble dy = p.m_y - GetBottom();
    if(isFixed()) {
        if(p.m_x <= GetLeft() && p.m_y <= GetBottom()) {
            dy = calcOrdi(-dx);
        } else if(p.m_x >= GetLeft() && p.m_y >= GetBottom()) {
            dx = calcAbsc(-dy);
        } else if(p.m_x > GetLeft() && p.m_y < GetBottom()) {
            wxDouble proy = dx / -dy;
            if(proy > aspectRatio) dx = calcAbsc(-dy);
            else dy = calcOrdi(-dx);
        } else if(p.m_x < GetLeft() && p.m_y > GetBottom()) {
            wxDouble proy = dx / -dy;
            if(proy < aspectRatio) dx = calcAbsc(-dy);
            else dy = calcOrdi(-dx);
        }
    }
    m_x += dx;
    m_width -= dx;
    m_height += dy;
    if(isCentered()) {
        m_width -= dx;
        m_y -= dy;
        m_height += dy;
    }
}

void SmartRect::pushTopTo(const wxDouble &p) {
    if(p == GetTop()) return;
    wxDouble dx = 0;
    wxDouble dy = p - GetTop();
    if(isFixed()) dx = calcAbsc(dy) / 2;
    m_x += dx;
    m_width -= (dx * 2);
    m_y += dy;
    m_height -= dy;
    if(isCentered()) {
        m_x += dx;
        m_width -= (dx * 2);
        m_height -= dy;
    }
}

void SmartRect::pushBottomTo(const wxDouble &p) {
    if(p == GetBottom()) return;
    wxDouble dx = 0;
    wxDouble dy = p - GetBottom();
    if(isFixed()) dx = calcAbsc(dy) / 2;
    m_x -= dx;
    m_width += dx * 2;
    m_height += dy;
    if(isCentered()) {
        m_x -= dx;
        m_width += dx * 2;
        m_y -= dy;
        m_height += dy;
    }
}

void SmartRect::pushLeftTo(const wxDouble &p) {
    if(p == GetLeft()) return;
    wxDouble dx = p - GetLeft();
    wxDouble dy = 0;
    if(isFixed()) dy = calcOrdi(dx) / 2;
    m_x += dx;
    m_y += dy;
    m_width -= dx;
    m_height -= dy * 2;
    if(isCentered()) {
        m_width -= dx;
        m_y += dy;
        m_height -= dy * 2;
    }
}

void SmartRect::pushRightTo(const wxDouble &p) {
    if(p == GetRight()) return;
    wxDouble dx = p - GetRight();
    wxDouble dy = 0;
    if(isFixed()) dy = calcOrdi(dx) / 2;
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
    rx = r.m_x;
    ry = r.m_y;
    rw = r.m_width;
    ry = r.m_height;
    return restrict(restricted);
}

void SmartRect::setMark() {
    mx = m_x;
    my = m_y;
    mw = m_width;
    mh = m_height;
}

void SmartRect::useMark() {
    m_x = mx;
    m_y = my;
    m_width = mw;
    m_height = mh;
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
    ix = extGetLeft();
    iy = extGetTop();
    iw = extGetWidth();
    ih = extGetHeight();
}

bool SmartRect::instantChanged() {
    return ix != extGetLeft() ||
            iy != extGetTop() ||
            iw != extGetWidth() ||
            ih != extGetHeight();
}

SmartRect::~SmartRect() {

}
