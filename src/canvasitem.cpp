#include "canvasitem.hpp"
#include "defs.hpp"
#include "scaler.hpp"
#include <wx/dcmemory.h>
#include <wx/gdicmn.h>
#include "extendedcanvas.hpp"
#include "recthelper.hpp"
#include <iostream>
#include <iomanip>

CanvasItem::CanvasItem() : CanvasItem(-1, wxRect2DDouble(0, 0, 1, 1)) {

}

CanvasItem::CanvasItem(int id, wxRect2DDouble geometry) {
    this->id = id;
    this->geometry = geometry;
    this->scaler = nullptr;
    this->hidden = false;
    this->locked = true;
    this->selected = false;
    this->container = nullptr;
    this->hover = ict::NONE_ZONE;
    hdim = 15;
}

CanvasItem::~CanvasItem() {

}

wxDouble CanvasItem::extGetLeft() const {
    if(useGrid()) return round_rb(geometry.GetLeft());
    else return geometry.GetLeft();
}

wxDouble CanvasItem::extGetTop() const {
    if(useGrid()) return round_rb(geometry.GetTop());
    else return geometry.GetTop();
}

wxDouble CanvasItem::extGetRight() const {
    if(useGrid()) return round_lb(geometry.GetRight());
    else return geometry.GetRight();
}

wxDouble CanvasItem::extGetBottom() const {
    if(useGrid()) return round_lb(geometry.GetBottom());
    else return geometry.GetBottom();
}

wxDouble CanvasItem::extGetWidth() const {
    if(useGrid()) return extGetRight() - extGetLeft();
    else return geometry.m_width;
}

wxDouble CanvasItem::extGetHeight() const {
    if(useGrid()) return extGetBottom() - extGetTop();
    else return geometry.m_height;
}

wxRect2DDouble CanvasItem::extGetRect() const {
    return wxRect2DDouble(extGetLeft(), extGetTop(),
                          extGetWidth(), extGetHeight());
}

wxDouble CanvasItem::getWidth(ict::ECContext ic, bool ext) const {
    if (ic == ict::VIRTUAL_CONTEXT) {
        if(ext) return extGetWidth();
        else return geometry.m_width;
    }
    else return scaler->scaleX(getWidth(ict::VIRTUAL_CONTEXT, ext), ict::IN_D);
}

wxDouble CanvasItem::getHeight(ict::ECContext ic, bool ext) const {
    if (ic == ict::VIRTUAL_CONTEXT) {
        if(ext) return extGetHeight();
        else return geometry.m_height;
    }
    else return scaler->scaleY(getHeight(ict::VIRTUAL_CONTEXT, ext), ict::IN_D);
}

wxDouble CanvasItem:: getRight(ict::ECContext ic, bool ext, bool ref) const {
    if (ic == ict::VIRTUAL_CONTEXT) {
        wxDouble ret;
        if(ext) ret = extGetRight();
        else ret = geometry.GetRight();
        if(ref) ret += getContainerReference(ic).m_x;
        return ret;
    }
    else return scaler->scaleX(getRight(ict::VIRTUAL_CONTEXT, ext, ref), ict::IN_D);
}

wxDouble CanvasItem::getBottom(ict::ECContext ic, bool ext, bool ref) const {
    if (ic == ict::VIRTUAL_CONTEXT) {
        wxDouble ret;
        if(ext) ret = extGetBottom();
        else ret = geometry.GetBottom();
        if(ref) ret += getContainerReference(ic).m_y;
        return ret;
    }
    else return scaler->scaleY(getBottom(ict::VIRTUAL_CONTEXT, ext, ref), ict::IN_D);
}

wxDouble CanvasItem::getLeft(ict::ECContext ic, bool ext, bool ref) const {
    if (ic == ict::VIRTUAL_CONTEXT) {
        wxDouble ret;
        if(ext) ret = extGetLeft();
        else ret = geometry.GetLeft();
        if(ref) ret += getContainerReference(ic).m_x;
        return ret;
    }
    else return scaler->scaleX(getLeft(ict::VIRTUAL_CONTEXT, ext, ref), ict::IN_D);
}

wxDouble CanvasItem::getTop(ict::ECContext ic, bool ext, bool ref) const {
    if (ic == ict::VIRTUAL_CONTEXT) {
        wxDouble ret;
        if(ext) ret = extGetTop();
        else ret = geometry.GetTop();
        if(ref) ret += getContainerReference(ic).m_y;
        return ret;
    }
    else return scaler->scaleY(getTop(ict::VIRTUAL_CONTEXT, ext, ref), ict::IN_D);
}

wxRect2DDouble CanvasItem::getHandleZone(int z) const {
    if(z == ict::RT_ZONE) {
        return wxRect2DDouble(getRight(ict::CANVAS_CONTEXT), getTop(ict::CANVAS_CONTEXT) - hdim, hdim, hdim);
    } else if(z == ict::LT_ZONE) {
        return wxRect2DDouble(getLeft(ict::CANVAS_CONTEXT) - hdim, getTop(ict::CANVAS_CONTEXT) - hdim, hdim, hdim);
    } else if(z == ict::RB_ZONE) {
        return wxRect2DDouble(getRight(ict::CANVAS_CONTEXT), getBottom(ict::CANVAS_CONTEXT), hdim, hdim);
    } else if(z == ict::LB_ZONE) {
        return wxRect2DDouble(getLeft(ict::CANVAS_CONTEXT) - hdim, getBottom(ict::CANVAS_CONTEXT), hdim, hdim);
    } else if(z == ict::T_ZONE) {
        return wxRect2DDouble(getLeft(ict::CANVAS_CONTEXT), getTop(ict::CANVAS_CONTEXT) - hdim, getWidth(ict::CANVAS_CONTEXT), hdim);
    } else if(z == ict::B_ZONE) {
        return wxRect2DDouble(getLeft(ict::CANVAS_CONTEXT), getBottom(ict::CANVAS_CONTEXT), getWidth(ict::CANVAS_CONTEXT), hdim);
    } else if(z == ict::R_ZONE) {
        return wxRect2DDouble(getRight(ict::CANVAS_CONTEXT), getTop(ict::CANVAS_CONTEXT), hdim, getHeight(ict::CANVAS_CONTEXT));
    } else if(z == ict::L_ZONE) {
        return wxRect2DDouble(getLeft(ict::CANVAS_CONTEXT) - hdim, getTop(ict::CANVAS_CONTEXT), hdim, getHeight(ict::CANVAS_CONTEXT));
    } else if(z == ict::IN_ZONE) {
        return getGeometry(ict::CANVAS_CONTEXT);
    } else return wxRect2DDouble(0, 0, 0, 0);
}

wxPoint2DDouble CanvasItem::relativeToEdge(const wxPoint2DDouble &p, int z, ict::ECContext c, bool ext, bool ref) {
    if(z == ict::RT_ZONE) {
        return wxPoint2DDouble(p.m_x - getRight(c, ext, ref), p.m_y - getTop(c, ext, ref));
    } else if(z == ict::LT_ZONE) {
        return wxPoint2DDouble(p.m_x - getLeft(c, ext, ref), p.m_y - getTop(c, ext, ref));
    } else if(z == ict::RB_ZONE) {
        return wxPoint2DDouble(p.m_x - getRight(c, ext, ref), p.m_y - getBottom(c, ext, ref));
    } else if(z == ict::LB_ZONE) {
        return wxPoint2DDouble(p.m_x - getLeft(c, ext, ref), p.m_y - getBottom(c, ext, ref));
    } else if(z == ict::T_ZONE) {
        return wxPoint2DDouble(0, p.m_y - getTop(c, ext, ref));
    } else if(z == ict::B_ZONE) {
        return wxPoint2DDouble(0, p.m_y - getBottom(c, ext, ref));
    } else if(z == ict::R_ZONE) {
        return wxPoint2DDouble(p.m_x - getRight(c, ext, ref), 0);
    } else if(z == ict::L_ZONE) {
        return wxPoint2DDouble(p.m_x - getLeft(c, ext, ref), 0);
    } else { // IN_ZONE
        return p - getPosition(c, ext, ref);
    }
}

wxPoint2DDouble CanvasItem::relativeToReference(const wxPoint2DDouble &p, ict::ECContext c) {
    return p - getContainerReference(c);
}

wxPoint2DDouble CanvasItem::getContainerReference(ict::ECContext c) const {
    if(container) return container->getReference(c);
    return wxPoint2DDouble(0, 0);
}

int CanvasItem::getHandler() const {
    return geometry.activatedZone();
}

int CanvasItem::inHandle(const wxPoint2DDouble &canvasPoint) const {
    if (selected) {
        if(getHandleZone(ict::RT_ZONE).Contains(canvasPoint)) return ict::RT_ZONE;
        if(getHandleZone(ict::LT_ZONE).Contains(canvasPoint)) return ict::LT_ZONE;
        if(getHandleZone(ict::RB_ZONE).Contains(canvasPoint)) return ict::RB_ZONE;
        if(getHandleZone(ict::LB_ZONE).Contains(canvasPoint)) return ict::LB_ZONE;
        if(getHandleZone(ict::T_ZONE).Contains(canvasPoint)) return ict::T_ZONE;
        if(getHandleZone(ict::B_ZONE).Contains(canvasPoint)) return ict::B_ZONE;
        if(getHandleZone(ict::R_ZONE).Contains(canvasPoint)) return ict::R_ZONE;
        if(getHandleZone(ict::L_ZONE).Contains(canvasPoint)) return ict::L_ZONE;
    }
    if(getHandleZone(ict::IN_ZONE).Contains(canvasPoint)) return ict::IN_ZONE;
    return ict::NONE_ZONE;
}

int CanvasItem::press(const wxPoint &canvasPoint) {
    if(locked) return ict::NONE_ZONE;
    int handler = inHandle(canvasPoint);
    geometry.activateZone(handler);
    if(!handler) return handler;
    lastPoint = relativeToReference(canvasPoint, ict::CANVAS_CONTEXT);
    lastPoint = scaler->scalePoint(lastPoint, ict::OUT_D);
    rPressure = relativeToEdge(canvasPoint, handler, ict::CANVAS_CONTEXT, false);
    rPressure = scaler->scalePoint(rPressure, ict::OUT_D);
    return handler;
}

void CanvasItem::release() {
    geometry.activateZone(ict::NONE_ZONE);
}

bool CanvasItem::doHover(int z) {
    if(hover != z) {
        sHover = hover;
        hover = z;
        return hover != sHover;
    }
    return false;
}

wxPoint2DDouble CanvasItem::getSize(ict::ECContext ic, bool ext) const {
    return wxPoint2DDouble(getWidth(ic, ext), getHeight(ic, ext));
}

wxPoint2DDouble CanvasItem::getPosition(ict::ECContext ic, bool ext, bool ref) const {
    return wxPoint2DDouble(getLeft(ic, ext, ref), getTop(ic, ext, ref));
}

wxRect2DDouble CanvasItem::getGeometry(ict::ECContext ic, bool ext, bool ref) const {
    wxPoint2DDouble p(getPosition(ic, ext, ref));
    wxPoint2DDouble s(getSize(ic, ext));
    return wxRect2DDouble(p.m_x, p.m_y, s.m_x, s.m_y);
}

wxRect2DDouble CanvasItem::getArea() const {
    wxRect2DDouble area(getGeometry(ict::CANVAS_CONTEXT));
    inflateRect(&area, hdim);
    return area;
}

bool CanvasItem::isRestricted() const {
    return geometry.isRestricted();
}

void CanvasItem::modify(const wxPoint &canvasPoint) {
    if(!geometry.activatedZone()) return;
    lastPoint = relativeToReference(canvasPoint, ict::CANVAS_CONTEXT);
    lastPoint = scaler->scalePoint(lastPoint, ict::OUT_D);
    virtualModify(lastPoint);
}

void CanvasItem::virtualModify(const wxPoint2DDouble &vp) {
    geometry.setZoneTo(vp - rPressure);
    hover = geometry.activatedZone();
    if(sGeometry != getGeometry(ict::CANVAS_CONTEXT)) {
        if(container) container->notifyGeometry(this);
        sGeometry = getGeometry(ict::CANVAS_CONTEXT);
    }
}

void CanvasItem::setVirtualRestriction(const wxRect2DDouble &restriction) {
    geometry.setRestriction(restriction);
}

void CanvasItem::restrict(bool opt) {
    geometry.restrict(opt);
}

void CanvasItem::setAspectRatio(int xr, int yr) {
    geometry.setAspectRatio(xr, yr);
}

void CanvasItem::useSavedMark() {
    if(!geometry.resizing()) return;
    geometry.useMark();
    virtualModify(lastPoint);
}

void CanvasItem::expandFromCenter(bool op) {
    geometry.expandFromCenter(op);
    useSavedMark();
}

void CanvasItem::fixedAspectRatio(bool op) {
    geometry.fixedAspectRatio(op);
    useSavedMark();
}

bool CanvasItem::expandFromCenter() const {
    return geometry.expandFromCenter();
}

bool CanvasItem::fixedAspectRatio() const {
    return geometry.fixedAspectRatio();
}

wxRect2DDouble CanvasItem::getAreaUpdate() const {
    wxRect2DDouble updArea(sGeometry.CreateUnion(getGeometry(ict::CANVAS_CONTEXT)));
    inflateRect(&updArea, hdim);
    return updArea;
}

wxRect2DDouble CanvasItem::getHoverUpdate() const {
    return getHandleZone(hover).CreateUnion(getHandleZone(sHover));
}

void CanvasItem::setVirtualSize(const wxPoint2DDouble &dim) {
    geometry.setSize(dim);
}

void CanvasItem::setVirtualGeometry(const wxRect2DDouble &geo) {
    geometry.setGeometry(geo);
}

void CanvasItem::setVirtualPosition(const wxPoint2DDouble &pos) {
    geometry.setPosition(pos);
}

wxDouble CanvasItem::getAspectRatio() const {
    return geometry.getAspectRatio();
}

void CanvasItem::setContainer(ExtendedCanvas *c) {
    container = c;
}

void CanvasItem::drawOn(wxMemoryDC *pv) {
    drawHandles(pv);
    pv->SetBrush(*wxRED_BRUSH);
    wxRect2DDouble ddr(getGeometry(ict::CANVAS_CONTEXT));
    wxRect dr(ddr.m_x, ddr.m_y, ddr.m_width, ddr.m_height);
    pv->DrawRectangle(dr);
    if(hover) {
        pv->SetBrush(*wxYELLOW_BRUSH);
        wxRect2DDouble ddr(getHandleZone(hover));
        wxRect dr(ddr.m_x, ddr.m_y, ddr.m_width, ddr.m_height);
        pv->DrawRectangle(dr);
    }
}

void CanvasItem::drawHandles(wxMemoryDC *pv) {
    if (!selected) return;
    pv->SetBrush(*wxBLUE_BRUSH);
    wxRect2DDouble ddr(getArea());
    wxRect dr(ddr.m_x, ddr.m_y, ddr.m_width, ddr.m_height);
    pv->DrawRectangle(dr);
}

bool CanvasItem::operator==(const CanvasItem &c) {
    return getId() == c.getId();
}

bool CanvasItem::operator!=(const CanvasItem &c) {
    return getId() != c.getId();
}

int CanvasItem::getId() const {
    return id;
}

void CanvasItem::setScaler(Scaler *s) {
    scaler = s;
}

void CanvasItem::select(const bool opt) {
    if(selected != opt) {
        selected = opt;
        if(container) container->notifySelection(this);
    }
}

bool CanvasItem::isSelected() const {
    return selected;
}

void CanvasItem::lock(const bool opt) {
    locked = opt;
}

bool CanvasItem::isLocked() {
    return locked;
}

void CanvasItem::hide(bool opt) {
    hidden = opt;
}

bool CanvasItem::isHidden() const {
    return hidden;
}

void CanvasItem::useGrid(bool op) {
    geometry.looseRestriction(op);
}

bool CanvasItem::useGrid() const {
    return geometry.looseRestriction();
}
