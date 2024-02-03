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
#include "scrolledcanvas.hpp"
#include <wx/dcmemory.h>
#include <wx/gdicmn.h>
#include <iostream>

CanvasItem::CanvasItem() : CanvasItem(-1, wxRect2DDouble(0, 0, 1, 1)) {

}

CanvasItem::CanvasItem(int id, wxRect2DDouble geometry) {
    this->id = id;
    this->geometry = geometry;
    this->scaler = nullptr;
    this->locked = true;
    this->selected = false;
    this->zonePressed = ict::NONE;
    this->reference = nullptr;
}

CanvasItem::~CanvasItem() {

}

void CanvasItem::setVirtualReference(wxPoint2DDouble *r) {
    reference = r;
}

wxDouble CanvasItem::getX(ItemContext ic, bool unref) const {
    if (ic == VIRTUAL_CONTEXT) {
        wxDouble rx = 0.0;
        if (reference && !unref) rx = reference->m_x;
        return geometry.m_x + rx;
    }
    else return scaler->scaleX(getX(VIRTUAL_CONTEXT, unref), ict::IN_D);
}

wxDouble CanvasItem::getY(ItemContext ic, bool unref) const {
    if (ic == VIRTUAL_CONTEXT) {
        wxDouble ry = 0.0;
        if (reference && !unref) ry = reference->m_y;
        return geometry.m_y + ry;
    }
    else return scaler->scaleY(getY(VIRTUAL_CONTEXT, unref), ict::IN_D);
}

wxDouble CanvasItem::getWidth(ItemContext ic) const {
    if (ic == VIRTUAL_CONTEXT) return geometry.m_width;
    else return scaler->scaleX(geometry.m_width, ict::IN_D);
}

wxDouble CanvasItem::getHeight(ItemContext ic) const {
    if (ic == VIRTUAL_CONTEXT) return geometry.m_height;
    else return scaler->scaleY(geometry.m_height, ict::IN_D);
}

wxDouble CanvasItem:: getRight(ItemContext ic, bool unref) const {
    return getLeft(ic, unref) + getWidth(ic);
}

wxDouble CanvasItem::getBottom(ItemContext ic, bool unref) const {
    return getTop(ic, unref) + getHeight(ic);
}

wxDouble CanvasItem::getLeft(ItemContext ic, bool unref) const {
    return getX(ic, unref);
}

wxDouble CanvasItem::getTop(ItemContext ic, bool unref) const {
    return getY(ic, unref);
}

wxRect2DDouble CanvasItem::getZone(ict::ItemZone z) const {
    if(z == ict::NE) {
        return wxRect2DDouble(getRight(CANVAS_CONTEXT), getTop(CANVAS_CONTEXT) - ict::CORNER, ict::CORNER, ict::CORNER);
    } else if(z == ict::NW) {
        return wxRect2DDouble(getLeft(CANVAS_CONTEXT) - ict::CORNER, getTop(CANVAS_CONTEXT) - ict::CORNER, ict::CORNER, ict::CORNER);
    } else if(z == ict::SE) {
        return wxRect2DDouble(getRight(CANVAS_CONTEXT), getBottom(CANVAS_CONTEXT), ict::CORNER, ict::CORNER);
    } else if(z == ict::SW) {
        return wxRect2DDouble(getLeft(CANVAS_CONTEXT) - ict::CORNER, getBottom(CANVAS_CONTEXT), ict::CORNER, ict::CORNER);
    } else if(z == ict::N) {
        return wxRect2DDouble(getLeft(CANVAS_CONTEXT), getTop(CANVAS_CONTEXT) - ict::CORNER, getWidth(CANVAS_CONTEXT), ict::CORNER);
    } else if(z == ict::S) {
        return wxRect2DDouble(getLeft(CANVAS_CONTEXT), getBottom(CANVAS_CONTEXT), getWidth(CANVAS_CONTEXT), ict::CORNER);
    } else if(z == ict::E) {
        return wxRect2DDouble(getRight(CANVAS_CONTEXT), getTop(CANVAS_CONTEXT), ict::CORNER, getHeight(CANVAS_CONTEXT));
    } else if(z == ict::W) {
        return wxRect2DDouble(getLeft(CANVAS_CONTEXT) - ict::CORNER, getTop(CANVAS_CONTEXT), ict::CORNER, getHeight(CANVAS_CONTEXT));
    } else if(z == ict::INNER)
        return getGeometry(CANVAS_CONTEXT);
    return wxRect2DDouble(0, 0, 0, 0);
}

wxPoint2DDouble CanvasItem::relativeToEdge(const wxPoint2DDouble &p, ict::ItemZone z, ItemContext c) {
    if(z == ict::NE) {
        return wxPoint2DDouble(p.m_x - getRight(c), p.m_y - getTop(c));
    } else if(z == ict::NW) {
        return wxPoint2DDouble(p.m_x - getLeft(c), p.m_y - getTop(c));
    } else if(z == ict::SE) {
        return wxPoint2DDouble(p.m_x - getRight(c), p.m_y - getBottom(c));
    } else if(z == ict::SW) {
        return wxPoint2DDouble(p.m_x - getLeft(c), p.m_y - getBottom(c));
    } else if(z == ict::N) {
        return wxPoint2DDouble(0, p.m_y - getTop(c));
    } else if(z == ict::S) {
        return wxPoint2DDouble(0, p.m_y - getBottom(c));
    } else if(z == ict::E) {
        return wxPoint2DDouble(p.m_x - getRight(c), 0);
    } else if(z == ict::W) {
        return wxPoint2DDouble(p.m_x - getLeft(c), 0);
    } else if(z == ict::INNER) {
        return p - getPosition(c);
    }
    return p - getReference(c);
}

wxPoint2DDouble CanvasItem::getReference(ItemContext c) {
    if(reference) {
        if(c == VIRTUAL_CONTEXT) return *reference;
        else return scaler->scalePoint(*reference, ict::IN_D);
    }
    return wxPoint2DDouble(0, 0);
}

ict::ItemZone CanvasItem::getZonePressed() const {
    return zonePressed;
}

ict::ItemZone CanvasItem::getLocation(const wxPoint2DDouble &canvasPoint) const {
    if (selected) {
        if(getZone(ict::NE).Contains(canvasPoint)) return ict::NE;
        if(getZone(ict::NW).Contains(canvasPoint)) return ict::NW;
        if(getZone(ict::SE).Contains(canvasPoint)) return ict::SE;
        if(getZone(ict::SW).Contains(canvasPoint)) return ict::SW;
        if(getZone(ict::N).Contains(canvasPoint)) return ict::N;
        if(getZone(ict::S).Contains(canvasPoint)) return ict::S;
        if(getZone(ict::E).Contains(canvasPoint)) return ict::E;
        if(getZone(ict::W).Contains(canvasPoint)) return ict::W;
    }
    if(getZone(ict::INNER).Contains(canvasPoint)) return ict::INNER;
    return ict::NONE;
}

ict::ItemZone CanvasItem::press(const wxPoint &canvasPoint) {
    if(locked) return ict::NONE;
    zonePressed = getLocation(canvasPoint);
    if (zonePressed == ict::NONE) {
        selected = false;
        return zonePressed;
    }
    selected = true;
    relativePress = relativeToEdge(canvasPoint, zonePressed, CANVAS_CONTEXT);
    relativePress = scaler->scalePoint(relativePress, ict::OUT_D);
    lastPoint = relativeToEdge(canvasPoint, ict::NONE, CANVAS_CONTEXT);
    lastPoint = scaler->scalePoint(lastPoint, ict::OUT_D);
    lastPoint -= relativePress;
    return zonePressed;
}

void CanvasItem::release() {
    zonePressed = ict::NONE;
    geometry.setMark();
}

wxPoint2DDouble CanvasItem::getDimensions(ItemContext ic) const {
    return wxPoint2DDouble(getWidth(ic), getHeight(ic));
}

wxPoint2DDouble CanvasItem::getPosition(ItemContext ic, bool unref) const {
    return wxPoint(getLeft(ic, unref), getTop(ic, unref));
}

wxRect2DDouble CanvasItem::getGeometry(ItemContext ic, bool unref) const {
    wxPoint2DDouble p(getPosition(ic));
    wxPoint2DDouble s(getDimensions(ic));
    return wxRect2DDouble(p.m_x, p.m_y, s.m_x, s.m_y);
}

wxRect2DDouble CanvasItem::getArea() const {
    wxRect2DDouble area(getGeometry(CANVAS_CONTEXT));
    area.SetLeft(area.GetLeft() - ict::CORNER);
    area.SetTop(area.GetTop() - ict::CORNER);
    area.SetRight(area.GetRight() + ict::CORNER);
    area.SetBottom(area.GetBottom() + ict::CORNER);
    return area;
}

bool CanvasItem::isRestricted() const {
    return geometry.isRestricted();
}

bool CanvasItem::modify(const wxPoint &canvasPoint) {
    if (zonePressed == ict::NONE) return false;
    lastPoint = relativeToEdge(canvasPoint, ict::NONE, CANVAS_CONTEXT);
    lastPoint = scaler->scalePoint(lastPoint, ict::OUT_D);
    lastPoint -= relativePress;
    bool changed;
    if (zonePressed == ict::INNER) {
        changed = geometry.pushTo(lastPoint);
    } else {
        changed = resize();
    }
    if (changed) return true;
    else return false;
}

/* void CanvasItem::move(const wxPoint2DDouble &target) {
    wxPoint2DDouble newPos(target - relativePress);
    if(newPos == geometry.GetPosition()) return;
    geometry.m_x = newPos.m_x;
    geometry.m_y = newPos.m_y;
    pushToRestriction();
} */

bool CanvasItem::resize() {
    if(zonePressed == ict::SE) {
        return geometry.pushRightBottomTo(lastPoint);
    }
    return false;
    /* wxDouble deltaX = - relativePress.m_x;
    wxDouble deltaY = - relativePress.m_y;
    if(zonePressed == ict::SE) {
        deltaX += target.m_x - (geometry.m_x + geometry.m_width);
        deltaY += target.m_y - (geometry.m_y + geometry.m_height);
        geometry = wxRect2DDouble(geometry.m_x, geometry.m_y, geometry.m_width + deltaX,
                geometry.m_height + deltaY);
    } else if(zonePressed == ict::NW) {
        deltaX += target.m_x - geometry.m_x;
        deltaY += target.m_y - geometry.m_y;
        geometry = wxRect2DDouble(geometry.m_x + deltaX, geometry.m_y + deltaY,
                geometry.m_width - deltaX,
                geometry.m_height - deltaY);
    } else if(zonePressed == ict::NE) {
        deltaX += target.m_x - (geometry.m_x + geometry.m_width);
        deltaY += target.m_y - geometry.m_y;
        geometry = wxRect2DDouble(geometry.m_x, geometry.m_y + deltaY,
                geometry.m_width + deltaX,
                geometry.m_height - deltaY);
    } else if(zonePressed == ict::SW) {
        deltaX += target.m_x - geometry.m_x;
        deltaY += target.m_y - (geometry.m_y + geometry.m_height);
        geometry = wxRect2DDouble(geometry.m_x + deltaX, geometry.m_y,
                geometry.m_width - deltaX,
                geometry.m_height + deltaY);
    } else if(zonePressed == ict::N) {
        deltaY += target.m_y - geometry.m_y;
        geometry = wxRect2DDouble(geometry.m_x + deltaX, geometry.m_y + deltaY,
                geometry.m_width - deltaX,
                geometry.m_height - deltaY);
    } else if(zonePressed == ict::S) {
        deltaY += target.m_y - (geometry.m_y + geometry.m_height);
        geometry = wxRect2DDouble(geometry.m_x, geometry.m_y,
                geometry.m_width + deltaX,
                geometry.m_height + deltaY);
    } else if(zonePressed == ict::W) {
        deltaX += target.m_x - geometry.m_x;
        geometry = wxRect2DDouble(geometry.m_x + deltaX, geometry.m_y,
                geometry.m_width - deltaX,
                geometry.m_height + deltaY);
    } else if(zonePressed == ict::E) {
        deltaX += target.m_x - (geometry.m_x + geometry.m_width);
        geometry = wxRect2DDouble(geometry.m_x, geometry.m_y + deltaY,
                geometry.m_width + deltaX,
                geometry.m_height - deltaY);
    }
    fitInRestriction(zonePressed); */
}

/* void CanvasItem::pushToRestriction() {
    if(!isRestricted() || restriction.Contains(geometry)) return;
    int cx1 = restriction.GetLeft(), cx2 = restriction.GetRight();
    int nx1 = geometry.GetLeft(), nx2 = geometry.GetRight();
    if(nx2 > cx2) modifyPosition(cx2 - geometry.m_width, geometry.m_y);
    if(nx1 < cx1) modifyPosition(cx1, geometry.m_y);
    int cy1 = restriction.m_y, cy2 = restriction.GetBottom();
    int ny1 = geometry.m_y, ny2 = geometry.GetBottom();
    if(ny2 > cy2) modifyPosition(geometry.m_x, cy2 - geometry.m_height);
    if(ny1 < cy1) modifyPosition(geometry.m_x, cy1);
} */

bool CanvasItem::setVirtualRestriction(const wxRect2DDouble &restriction) {
    return geometry.setRestriction(restriction);
}

bool CanvasItem::restrict(bool opt) {
    return geometry.restrict(opt);
}

void CanvasItem::setAspectRatio(int xr, int yr) {
    geometry.setAspectRatio(xr, yr);
}

void CanvasItem::expandFromCenter(bool op) {
    geometry.expandFromCenter(op);
}

void CanvasItem::fixedAspectRatio(bool op) {
    geometry.fixedAspectRatio(op);
    if (zonePressed == ict::NONE || zonePressed == ict::INNER) return;
    if (op) geometry.useMark();
    resize();
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

void CanvasItem::drawOn(wxMemoryDC *pv) {
    drawEntries(pv);
    pv->SetBrush(*wxRED_BRUSH);
    wxRect2DDouble ddr(getGeometry(CANVAS_CONTEXT));
    wxRect dr(ddr.m_x, ddr.m_y, ddr.m_width, ddr.m_height);
    pv->DrawRectangle(dr);
}

void CanvasItem::drawEntries(wxMemoryDC *pv) {
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
    selected = opt;
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
