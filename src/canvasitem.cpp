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
    this->handler = ict::NONE_ZONE;
    this->container = nullptr;
    this->handleHover = ict::NONE_ZONE;
    hdim = 15;
}

CanvasItem::~CanvasItem() {

}

wxDouble CanvasItem::getWidth(ict::ECContext ic, bool ext) const {
    if (ic == ict::VIRTUAL_CONTEXT) {
        if(ext) return geometry.extGetWidth();
        else return geometry.m_width;
    }
    else return scaler->scaleX(getWidth(ict::VIRTUAL_CONTEXT), ict::IN_D);
}

wxDouble CanvasItem::getHeight(ict::ECContext ic, bool ext) const {
    if (ic == ict::VIRTUAL_CONTEXT) {
        if(ext) return geometry.extGetHeight();
        else return geometry.m_height;
    }
    else return scaler->scaleY(getHeight(ict::VIRTUAL_CONTEXT), ict::IN_D);
}

wxDouble CanvasItem:: getRight(ict::ECContext ic, bool ext) const {
    if (ic == ict::VIRTUAL_CONTEXT) {
        if(ext) return getContainerReference(ic).m_x + geometry.extGetRight();
        else return getContainerReference(ic).m_x + geometry.GetRight();
    }
    else return scaler->scaleX(getRight(ict::VIRTUAL_CONTEXT, ext), ict::IN_D);
}

wxDouble CanvasItem::getBottom(ict::ECContext ic, bool ext) const {
    if (ic == ict::VIRTUAL_CONTEXT) {
        if(ext) return getContainerReference(ic).m_y + geometry.extGetBottom();
        else return getContainerReference(ic).m_y + geometry.GetBottom();
    }
    else return scaler->scaleY(getBottom(ict::VIRTUAL_CONTEXT, ext), ict::IN_D);
}

wxDouble CanvasItem::getLeft(ict::ECContext ic, bool ext) const {
    if (ic == ict::VIRTUAL_CONTEXT) {
        if(ext) return getContainerReference(ic).m_x + geometry.extGetLeft();
        else return getContainerReference(ic).m_x + geometry.GetLeft();
    }
    else return scaler->scaleX(getLeft(ict::VIRTUAL_CONTEXT, ext), ict::IN_D);
}

wxDouble CanvasItem::getTop(ict::ECContext ic, bool ext) const {
    if (ic == ict::VIRTUAL_CONTEXT) {
        if(ext) return getContainerReference(ic).m_y + geometry.extGetTop();
        else return getContainerReference(ic).m_y + geometry.GetTop();
    }
    else return scaler->scaleY(getTop(ict::VIRTUAL_CONTEXT, ext), ict::IN_D);
}

wxRect2DDouble CanvasItem::getZone(int z) const {
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

wxPoint2DDouble CanvasItem::relativeToEdge(const wxPoint2DDouble &p, int z, ict::ECContext c) {
    if(z == ict::RT_ZONE) {
        return wxPoint2DDouble(p.m_x - getRight(c, false), p.m_y - getTop(c, false));
    } else if(z == ict::LT_ZONE) {
        return wxPoint2DDouble(p.m_x - getLeft(c, false), p.m_y - getTop(c, false));
    } else if(z == ict::RB_ZONE) {
        return wxPoint2DDouble(p.m_x - getRight(c, false), p.m_y - getBottom(c, false));
    } else if(z == ict::LB_ZONE) {
        return wxPoint2DDouble(p.m_x - getLeft(c, false), p.m_y - getBottom(c, false));
    } else if(z == ict::T_ZONE) {
        return wxPoint2DDouble(0, p.m_y - getTop(c, false));
    } else if(z == ict::B_ZONE) {
        return wxPoint2DDouble(0, p.m_y - getBottom(c, false));
    } else if(z == ict::R_ZONE) {
        return wxPoint2DDouble(p.m_x - getRight(c, false), 0);
    } else if(z == ict::L_ZONE) {
        return wxPoint2DDouble(p.m_x - getLeft(c, false), 0);
    } else if(z == ict::IN_ZONE) {
        return p - getPosition(c);
    } else return p - getContainerReference(c);
}

wxPoint2DDouble CanvasItem::getContainerReference(ict::ECContext c) const {
    if(container) return container->getReference(c);
    return wxPoint2DDouble(0, 0);
}

int CanvasItem::getHandler() const {
    return handler;
}

bool CanvasItem::collides(const wxPoint2DDouble &p) {
    if(locked || hidden) {
        handleCollision = ict::NONE_ZONE;
        return false;
    }
    handleCollision = inHandle(p);
    if(!handleCollision) return false;
    if(container) container->notifyCollision(this);
    return true;
}

int CanvasItem::inHandle(const wxPoint2DDouble &canvasPoint) const {
    if (selected) {
        if(getZone(ict::RT_ZONE).Contains(canvasPoint)) return ict::RT_ZONE;
        if(getZone(ict::LT_ZONE).Contains(canvasPoint)) return ict::LT_ZONE;
        if(getZone(ict::RB_ZONE).Contains(canvasPoint)) return ict::RB_ZONE;
        if(getZone(ict::LB_ZONE).Contains(canvasPoint)) return ict::LB_ZONE;
        if(getZone(ict::T_ZONE).Contains(canvasPoint)) return ict::T_ZONE;
        if(getZone(ict::B_ZONE).Contains(canvasPoint)) return ict::B_ZONE;
        if(getZone(ict::R_ZONE).Contains(canvasPoint)) return ict::R_ZONE;
        if(getZone(ict::L_ZONE).Contains(canvasPoint)) return ict::L_ZONE;
    }
    if(getZone(ict::IN_ZONE).Contains(canvasPoint)) return ict::IN_ZONE;
    return ict::NONE_ZONE;
}

int CanvasItem::press(const wxPoint &canvasPoint) {
    if(locked) return ict::NONE_ZONE;
    cPoint = canvasPoint;
    handler = inHandle(canvasPoint);
    if(!handler) return handler;
    geometry.setMark();
    relativePress = relativeToEdge(canvasPoint, handler, ict::CANVAS_CONTEXT);
    relativePress = scaler->scalePoint(relativePress, ict::OUT_D);
    relativePoint = relativeToEdge(canvasPoint, ict::NONE_ZONE, ict::CANVAS_CONTEXT);
    relativePoint = scaler->scalePoint(relativePoint, ict::OUT_D);
    relativePoint -= relativePress;
    if(container) container->notifyPressure(this);
    return handler;
}

void CanvasItem::release() {
    handler = ict::NONE_ZONE;
}

void CanvasItem::hoverCollision() {
    hover(handleCollision);
}

void CanvasItem::hover(int z) {
    if(handleHover != z) {
        prevHover = handleHover;
        handleHover = z;
        if(container) container->notifyHover(this);
    }
}

wxPoint2DDouble CanvasItem::getDimensions(ict::ECContext ic) const {
    return wxPoint2DDouble(getWidth(ic), getHeight(ic));
}

wxPoint2DDouble CanvasItem::getPosition(ict::ECContext ic) const {
    return wxPoint2DDouble(getLeft(ic), getTop(ic));
}

wxRect2DDouble CanvasItem::getGeometry(ict::ECContext ic) const {
    wxPoint2DDouble p(getPosition(ic));
    wxPoint2DDouble s(getDimensions(ic));
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

bool CanvasItem::modify(const wxPoint &canvasPoint) {
    if(!handler) return false;
    cPoint = canvasPoint;
    relativePoint = relativeToEdge(canvasPoint, ict::NONE_ZONE, ict::CANVAS_CONTEXT);
    relativePoint = scaler->scalePoint(relativePoint, ict::OUT_D);
    relativePoint -= relativePress;
    bool changed = geometry.pushZoneTo(handler, relativePoint);
    handler = geometry.getLastZone();
    handleHover = handler;
    if(changed) {
        if(container) container->notifyGeometry(this);
        return true;
    }
    else return false;
}

bool CanvasItem::setVirtualRestriction(const wxRect2DDouble &restriction) {
    return geometry.setRestriction(restriction);
}

bool CanvasItem::restrict(bool opt) {
    return geometry.restrict(opt);
}

void CanvasItem::setAspectRatio(int xr, int yr) {
    geometry.setAspectRatio(xr, yr);
}

void CanvasItem::useSavedMark() {
    if(!handler || handler == ict::IN_ZONE) return;
    geometry.useMark();
    geometry.saveBefore(false);
    modify(cPoint);
    geometry.saveBefore(true);
}

void CanvasItem::expandFromCenter(bool op) {
    geometry.expandFromCenter(op);
    useSavedMark();
}

void CanvasItem::fixedAspectRatio(bool op) {
    geometry.fixedAspectRatio(op);
    useSavedMark();
}

wxRect2DDouble CanvasItem::getUpdateArea() const {
    wxRect2DDouble updArea(geometry.getChangeUnion());
    if(container) {
        updArea.m_x += container->getReference(ict::VIRTUAL_CONTEXT).m_x;
        updArea.m_y += container->getReference(ict::VIRTUAL_CONTEXT).m_y;
    }
    scaler->scaleRect(&updArea, ict::IN_D);
    inflateRect(&updArea, hdim);
    return updArea;
}

wxRect2DDouble CanvasItem::getHoverUpdate() const {
    return getZone(handleHover).CreateUnion(getZone(prevHover));
}
bool CanvasItem::setVirtualDimensions(const wxPoint2DDouble &dim) {
    return geometry.setSize(dim);
}

bool CanvasItem::setVirtualGeometry(const wxRect2DDouble &geo) {
    return geometry.setGeometry(geo);
}

bool CanvasItem::setVirtualPosition(const wxPoint2DDouble &pos) {
    return geometry.setPosition(pos);
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
    if(handleHover) {
        pv->SetBrush(*wxYELLOW_BRUSH);
        wxRect2DDouble ddr(getZone(handleHover));
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
    geometry.useGrid(op);
}

bool CanvasItem::useGrid() const {
    return geometry.useGrid();
}
