#include "smartrect.hpp"

SmartRect::SmartRect() : SmartRect(wxRect2DDouble(0, 0, 1, 1)) {

}

SmartRect::SmartRect(const wxRect2DDouble &r) {
    *this = r;
    this->fixed = false;
    this->restricted = false;
    this->centered = false;
}

bool SmartRect::setGeometry(const wxRect2DDouble &r) {
    saveInstant();
    *this = r;
    setMark();
    if(isRestricted()) {
        if(isCentered() && !restriction.Contains(GetCentre()))
            SetCentre(restriction.GetCentre());
        if(!isCentered() && !restriction.Contains(GetPosition())) {
            modifyPosition(restriction.m_x, restriction.m_y);
        }
        pushRightBottomTo(GetRightBottom());
    }
    return instantChanged();
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

void SmartRect::pushPointToPlayground(wxPoint2DDouble *p, bool inner) const {
    wxRect2DDouble playground;
    if(inner) playground = getInnerPlayground();
    else playground = getPlayground();
    if(playground.Contains(*p)) return;
    if(p->m_x < playground.GetLeft()) p->m_x = playground.GetLeft();
    if(p->m_x > playground.GetRight()) p->m_x = playground.GetRight();
    if(p->m_y < playground.GetTop()) p->m_y = playground.GetTop();
    if(p->m_y > playground.GetBottom()) p->m_y = playground.GetBottom();
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

bool SmartRect::pushTo(const wxPoint2DDouble &p) {
    if(p == GetPosition()) return false;
    wxPoint2DDouble inp(p);
    saveInstant();
    if(isRestricted()) pushPointToPlayground(&inp, true);
    modifyPosition(inp.m_x, inp.m_y);
    return instantChanged();
}

bool SmartRect::pushRightBottomTo(const wxPoint2DDouble &p) {
    saveInstant();
    wxPoint2DDouble inp(p);
    if(isRestricted()) pushPointToPlayground(&inp);
    /* lastReflec = ict::NONE_REFLEC;
    if(inp.m_x < GetLeft()) lastReflec ^= ict::HORI_REFLEC;
    if(inp.m_y < GetTop()) lastReflec ^= ict::VERT_REFLEC; */
    wxDouble deltaX = inp.m_x - (GetRight());
    wxDouble deltaY = inp.m_y - (GetBottom());
    if(isFixed()) {
        wxDouble proy = deltaX / deltaY;
        if(inp.m_x >= GetRight() && inp.m_y <= GetBottom()) {
            deltaX = calcAbsc(deltaY);
        } else if(inp.m_x <= GetRight() && inp.m_y >= GetBottom()) {
            deltaY = calcOrdi(deltaX);
        } else if(inp.m_x <= GetRight() && inp.m_y <= GetBottom()) {
            if(proy < aspectRatio) deltaX = calcAbsc(deltaY);
            else deltaY = calcOrdi(deltaX);
        } else if(inp.m_x >= GetRight() && inp.m_y >= GetBottom()) {
            if(proy > aspectRatio) deltaX = calcAbsc(deltaY);
            else deltaY = calcOrdi(deltaX);
        }
        /* if(lastReflec == ict::VERT_REFLEC) deltaX = -deltaX;
        if(lastReflec == ict::HORI_REFLEC) deltaY = -deltaY; */
    }
    m_width += deltaX;
    m_height += deltaY;
    if(isCentered()) {
        m_x -= deltaX; m_width += deltaX;
        m_y -= deltaY; m_height += deltaY;
    }
    // if(lastReflec) tryReflection();
    return instantChanged();
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

bool SmartRect::instantChanged() const {
    if (instant != *this) return true;
    else return false;
}

SmartRect &SmartRect::operator=(const wxRect2DDouble &r) {
    modifyGeometry(r.m_x, r.m_y, r.m_width, r.m_height);
    return *this;
}
SmartRect::~SmartRect() {

}
