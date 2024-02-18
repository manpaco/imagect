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
    this->accRight = 0;
    this->accBottom = 0;
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
    return GetRight() + accRight;
}

wxDouble SmartRect::internalGetBottom() const {
    return GetBottom() + accBottom;
}

wxPoint2DDouble SmartRect::internalGetCentre() const {
    return wxPoint2DDouble( (internalGetRight() + internalGetLeft()) / 2,
            (internalGetBottom() + internalGetTop()) / 2);
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
    accLeft = accTop = accRight = accBottom = 0;
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
    m_width = accumulateRight(re) - m_x;
    m_height = accumulateBottom(be) - m_y;
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

int SmartRect::getLastZone() const {
    return lastZone;
}

wxRect2DDouble SmartRect::getChangeUnion() const {
    return instant.CreateUnion(*this);
}

bool SmartRect::pushZoneTo(const int z, const wxPoint2DDouble &p) {
    saveInstant();
    lastZone = z;
    wxPoint2DDouble inp(p);
    wxDouble limitx, limity;
    if(isCentered()) {
        wxPoint2DDouble icen(internalGetCentre());
        limitx = icen.m_x;
        limity = icen.m_y;
    }
    switch (lastZone) {
        case ict::RB_ZONE: {
            if(!isCentered()) {
                limitx = internalGetLeft();
                limity = internalGetTop();
            }
            if(inp.m_x < limitx) {
                reflection ^= lastZone ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x - m_width;
            }
            if(inp.m_y < limity) {
                reflection ^= lastZone ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y - m_height;
            }
            break;
        }
        case ict::LT_ZONE: {
            if(!isCentered()) {
                limitx = internalGetRight();
                limity = internalGetBottom();
            }
            if(inp.m_x > limitx) {
                reflection ^= lastZone ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x + m_width;
            }
            if(inp.m_y > limity) {
                reflection ^= lastZone ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y + m_height;
            }
            break;
        }
        case ict::RT_ZONE: {
            if(!isCentered()) {
                limitx = internalGetLeft();
                limity = internalGetBottom();
            }
            if(inp.m_x < limitx) {
                reflection ^= lastZone ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x - m_width;
            }
            if(inp.m_y > limity) {
                reflection ^= lastZone ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y + m_height;
            }
            break;
        }
        case ict::LB_ZONE: {
            if(!isCentered()) {
                limitx = internalGetRight();
                limity = internalGetTop();
            }
            if(inp.m_x > limitx) {
                reflection ^= lastZone ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x + m_width;
            }
            if(inp.m_y < limity) {
                reflection ^= lastZone ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y - m_height;
            }
            break;
        }
        case ict::T_ZONE: {
            if(!isCentered()) limity = internalGetBottom();
            if(inp.m_y > limity) {
                reflection ^= lastZone ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y + m_height;
            }
            break;
        }
        case ict::B_ZONE: {
            if(!isCentered()) limity = internalGetTop();
            if(inp.m_y < limity) {
                reflection ^= lastZone ^= ict::VERT_REFLEC;
                if(!isCentered()) m_y = m_y - m_height;
            }
            break;
        }
        case ict::L_ZONE: {
            if(!isCentered()) limitx = internalGetRight();
            if(inp.m_x > limitx) {
                reflection ^= lastZone ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x + m_width;
            }
            break;
        }
        case ict::R_ZONE: {
            if(!isCentered()) limitx = internalGetLeft();
            if(inp.m_x < limitx) {
                reflection ^= lastZone ^= ict::HORI_REFLEC;
                if(!isCentered()) m_x = m_x - m_width;
            }
            break;
        }
    }
    // if(isRestricted()) placeInPlayground(&inp, lastZone == ict::IN_ZONE);
    switch (lastZone) {
        case ict::IN_ZONE:
            return pushTo(inp);
            break;
        case ict::RB_ZONE:
            return pushRightBottomTo(inp);
            break;
        case ict::LT_ZONE:
            return pushLeftTopTo(inp);
            break;
        case ict::RT_ZONE:
            return pushRightTopTo(inp);
            break;
        case ict::LB_ZONE:
            return pushLeftBottomTo(inp);
            break;
        case ict::T_ZONE:
            return pushTopTo(inp.m_y);
            break;
        case ict::B_ZONE:
            return pushBottomTo(inp.m_y);
            break;
        case ict::L_ZONE:
            return pushLeftTo(inp.m_x);
            break;
        case ict::R_ZONE:
            return pushRightTo(inp.m_x);
            break;
    }
    return false;
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
    wxDouble dx = p.m_x - internalGetRight();
    wxDouble dy = p.m_y - internalGetBottom();
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
        rdx = accumulateRight(dx);
        bdy = accumulateBottom(dy);
        m_width += rdx;
        m_height += bdy;
        if(isCentered()) {
            wxDouble ldx, tdy;
            ldx = accumulateLeft(-dx);
            tdy = accumulateTop(-dy);
            m_x += ldx; m_width -= ldx;
            m_y += tdy; m_height -= tdy;
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
    wxDouble dx = p.m_x - internalGetLeft();
    wxDouble dy = p.m_y - internalGetTop();
    if(isFixed()) {
        if(p.m_x >= internalGetLeft() && p.m_y <= internalGetTop()) {
            dy = calcOrdi(dx);
        } else if(p.m_x <= internalGetLeft() && p.m_y >= internalGetTop()) {
            dx = calcAbsc(dy);
        } else if(p.m_x < internalGetLeft() && p.m_y < internalGetTop()) {
            wxDouble proy = dx / dy;
            if(proy > aspectRatio) dx = calcAbsc(dy);
            else dy = calcOrdi(dx);
        } else if(p.m_x > internalGetLeft() && p.m_y > internalGetTop()) {
            wxDouble proy = dx / dy;
            if(proy < aspectRatio) dx = calcAbsc(dy);
            else dy = calcOrdi(dx);
        }
    }
    if(useGrid()) {
        wxDouble ldx, tdy;
        ldx = accumulateLeft(dx);
        tdy = accumulateTop(dy);
        m_x += ldx;
        m_y += tdy;
        m_width -= ldx;
        m_height -= tdy;
        if(isCentered()) {
            wxDouble rdx, bdy;
            rdx = accumulateRight(-dx);
            bdy = accumulateBottom(-dy);
            m_width += rdx;
            m_height += bdy;
        }
    } else {
        m_x += dx;
        m_y += dy;
        m_width -= dx;
        m_height -= dy;
        if(isCentered()) {
            m_width -= dx;
            m_height -= dy;
        }
    }
    return instantChanged();
}

bool SmartRect::pushRightTopTo(const wxPoint2DDouble &p) {
    wxDouble dx = p.m_x - internalGetRight();
    wxDouble dy = p.m_y - internalGetTop();
    if(isFixed()) {
        if(p.m_x <= internalGetRight() && p.m_y <= internalGetTop()) {
            dy = calcOrdi(-dx);
        } else if(p.m_x >= internalGetRight() && p.m_y >= internalGetTop()) {
            dx = calcAbsc(-dy);
        } else if(p.m_x > internalGetRight() && p.m_y < internalGetTop()) {
            wxDouble proy = dx / -dy;
            if(proy > aspectRatio) dx = calcAbsc(-dy);
            else dy = calcOrdi(-dx);
        } else if(p.m_x < internalGetRight() && p.m_y > internalGetTop()) {
            wxDouble proy = dx / -dy;
            if(proy < aspectRatio) dx = calcAbsc(-dy);
            else dy = calcOrdi(-dx);
        }
    }
    if(useGrid()) {
        wxDouble rdx, tdy;
        rdx = accumulateRight(dx);
        tdy = accumulateTop(dy);
        m_y += tdy;
        m_width += rdx;
        m_height -= tdy;
        if(isCentered()) {
            wxDouble ldx, bdy;
            ldx = accumulateLeft(-dx);
            bdy = accumulateBottom(-dy);
            m_x += ldx;
            m_width -= ldx;
            m_height += bdy;
        }
    } else {
        m_y += dy;
        m_width += dx;
        m_height -= dy;
        if(isCentered()) {
            m_x -= dx;
            m_width += dx;
            m_height -= dy;
        }
    }
    return instantChanged();
}

bool SmartRect::pushLeftBottomTo(const wxPoint2DDouble &p) {
    wxDouble dx = p.m_x - internalGetLeft();
    wxDouble dy = p.m_y - internalGetBottom();
    if(isFixed()) {
        if(p.m_x <= internalGetLeft() && p.m_y <= internalGetBottom()) {
            dx = calcAbsc(-dy);
        } else if(p.m_x >= internalGetLeft() && p.m_y >= internalGetBottom()) {
            dy = calcOrdi(-dx);
        } else if(p.m_x > internalGetLeft() && p.m_y < internalGetBottom()) {
            wxDouble proy = dx / -dy;
            if(proy < aspectRatio) dx = calcAbsc(-dy);
            else dy = calcOrdi(-dx);
        } else if(p.m_x < internalGetLeft() && p.m_y > internalGetBottom()) {
            wxDouble proy = dx / -dy;
            if(proy > aspectRatio) dx = calcAbsc(-dy);
            else dy = calcOrdi(-dx);
        }
    }
    if(useGrid()) {
        wxDouble ldx, bdy;
        ldx = accumulateLeft(dx);
        bdy = accumulateBottom(dy);
        m_x += ldx;
        m_width -= ldx;
        m_height += bdy;
        if(isCentered()) {
            wxDouble rdx, tdy;
            rdx = accumulateRight(-dx);
            tdy = accumulateTop(-dy);
            m_width += rdx;
            m_y += tdy;
            m_height -= tdy;
        }
    } else {
        m_x += dx;
        m_width -= dx;
        m_height += dy;
        if(isCentered()) {
            m_width -= dx;
            m_y -= dy;
            m_height += dy;
        }
    }
    return instantChanged();
}

bool SmartRect::pushTopTo(const wxDouble &p) {
    wxDouble dx = 0;
    wxDouble dy = p - internalGetTop();
    if(isFixed()) dx = calcAbsc(dy) / 2;
    if(useGrid()) {
        wxDouble tdy, ldx, rdx;
        ldx = accumulateLeft(dx);
        rdx = accumulateRight(-dx);
        m_x += ldx;
        m_width += rdx - ldx;
        tdy = accumulateTop(dy);
        m_y += tdy;
        m_height -= tdy;
        if(isCentered()) {
            ldx = accumulateLeft(dx);
            rdx = accumulateRight(-dx);
            m_x += ldx;
            m_width += rdx - ldx;
            tdy = accumulateBottom(-dy);
            m_height += tdy;
        }
    } else {
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
    return instantChanged();
}

bool SmartRect::pushBottomTo(const wxDouble &p) {
    wxDouble dx = 0;
    wxDouble dy = p - internalGetBottom();
    if(isFixed()) dx = calcAbsc(dy) / 2;
    if(useGrid()) {
        wxDouble bdy, ldx, rdx;
        ldx = accumulateLeft(-dx);
        rdx = accumulateRight(dx);
        m_x += ldx;
        m_width += rdx - ldx;
        bdy = accumulateBottom(dy);
        m_height += bdy;
        if(isCentered()) {
            ldx = accumulateLeft(-dx);
            rdx = accumulateRight(dx);
            m_x += ldx;
            m_width += rdx - ldx;
            bdy = accumulateTop(-dy);
            m_y += bdy;
            m_height -= bdy;
        }
    } else {
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
    return instantChanged();
}

bool SmartRect::pushLeftTo(const wxDouble &p) {
    wxDouble dx = p - internalGetLeft();
    wxDouble dy = 0;
    if(isFixed()) dy = calcOrdi(dx) / 2;
    if(useGrid()) {
        wxDouble ldx, tdy, bdy;
        ldx = accumulateLeft(dx);
        m_x += ldx;
        m_width -= ldx;
        bdy = accumulateBottom(-dy);
        tdy = accumulateTop(dy);
        m_y += tdy;
        m_height += bdy - tdy;
        if(isCentered()) {
            wxDouble rdx;
            rdx = accumulateRight(-dx);
            m_width += rdx;
            bdy = accumulateBottom(-dy);
            tdy = accumulateTop(dy);
            m_y += tdy;
            m_height += bdy - tdy;
        }
    } else {
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
    return instantChanged();
}

bool SmartRect::pushRightTo(const wxDouble &p) {
    wxDouble dx = p - internalGetRight();
    wxDouble dy = 0;
    if(isFixed()) dy = calcOrdi(dx) / 2;
    if(useGrid()) {
        wxDouble rdx, tdy, bdy;
        rdx = accumulateRight(dx);
        m_width += rdx;
        bdy = accumulateBottom(dy);
        tdy = accumulateTop(-dy);
        m_y += tdy;
        m_height += bdy - tdy;
        if(isCentered()) {
            wxDouble ldx;
            ldx = accumulateLeft(-dx);
            m_x += ldx;
            m_width -= ldx;
            bdy = accumulateBottom(dy);
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
    return instantChanged();
}

wxDouble SmartRect::accumulateLeft(const wxDouble &dl) {
    wxDouble ret;
    accLeft += dl;
    ret = round(accLeft);
    accLeft -= ret;
    return ret;
}

wxDouble SmartRect::accumulateTop(const wxDouble &dt) {
    wxDouble ret;
    accTop += dt;
    ret = round(accTop);
    accTop -= ret;
    return ret;
}

wxDouble SmartRect::accumulateRight(const wxDouble &dr) {
    wxDouble ret;
    accRight += dr;
    ret = round(accRight);
    accRight -= ret;
    return ret;
}

wxDouble SmartRect::accumulateBottom(const wxDouble &db) {
    wxDouble ret;
    accBottom += db;
    ret = round(accBottom);
    accBottom -= ret;
    return ret;
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

bool SmartRect::instantChanged() {
    if (instant != *this) return true;
    else return false;
}

SmartRect &SmartRect::operator=(const wxRect2DDouble &r) {
    modifyGeometry(r.m_x, r.m_y, r.m_width, r.m_height);
    return *this;
}

SmartRect::~SmartRect() {

}
