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
    this->zonePressed = ict::NONE_ZONE;
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

wxRect2DDouble CanvasItem::getZone(ict::RectZone z) const {
    if(z == ict::RT_ZONE) {
        return wxRect2DDouble(getRight(CANVAS_CONTEXT), getTop(CANVAS_CONTEXT) - ict::CORNER, ict::CORNER, ict::CORNER);
    } else if(z == ict::LT_ZONE) {
        return wxRect2DDouble(getLeft(CANVAS_CONTEXT) - ict::CORNER, getTop(CANVAS_CONTEXT) - ict::CORNER, ict::CORNER, ict::CORNER);
    } else if(z == ict::RB_ZONE) {
        return wxRect2DDouble(getRight(CANVAS_CONTEXT), getBottom(CANVAS_CONTEXT), ict::CORNER, ict::CORNER);
    } else if(z == ict::LB_ZONE) {
        return wxRect2DDouble(getLeft(CANVAS_CONTEXT) - ict::CORNER, getBottom(CANVAS_CONTEXT), ict::CORNER, ict::CORNER);
    } else if(z == ict::T_ZONE) {
        return wxRect2DDouble(getLeft(CANVAS_CONTEXT), getTop(CANVAS_CONTEXT) - ict::CORNER, getWidth(CANVAS_CONTEXT), ict::CORNER);
    } else if(z == ict::B_ZONE) {
        return wxRect2DDouble(getLeft(CANVAS_CONTEXT), getBottom(CANVAS_CONTEXT), getWidth(CANVAS_CONTEXT), ict::CORNER);
    } else if(z == ict::R_ZONE) {
        return wxRect2DDouble(getRight(CANVAS_CONTEXT), getTop(CANVAS_CONTEXT), ict::CORNER, getHeight(CANVAS_CONTEXT));
    } else if(z == ict::L_ZONE) {
        return wxRect2DDouble(getLeft(CANVAS_CONTEXT) - ict::CORNER, getTop(CANVAS_CONTEXT), ict::CORNER, getHeight(CANVAS_CONTEXT));
    } else if(z == ict::IN_ZONE)
        return getGeometry(CANVAS_CONTEXT);
    return wxRect2DDouble(0, 0, 0, 0);
}

wxPoint2DDouble CanvasItem::relativeToEdge(const wxPoint2DDouble &p, ict::RectZone z, ItemContext c) {
    if(z == ict::RT_ZONE) {
        return wxPoint2DDouble(p.m_x - getRight(c), p.m_y - getTop(c));
    } else if(z == ict::LT_ZONE) {
        return wxPoint2DDouble(p.m_x - getLeft(c), p.m_y - getTop(c));
    } else if(z == ict::RB_ZONE) {
        return wxPoint2DDouble(p.m_x - getRight(c), p.m_y - getBottom(c));
    } else if(z == ict::LB_ZONE) {
        return wxPoint2DDouble(p.m_x - getLeft(c), p.m_y - getBottom(c));
    } else if(z == ict::T_ZONE) {
        return wxPoint2DDouble(0, p.m_y - getTop(c));
    } else if(z == ict::B_ZONE) {
        return wxPoint2DDouble(0, p.m_y - getBottom(c));
    } else if(z == ict::R_ZONE) {
        return wxPoint2DDouble(p.m_x - getRight(c), 0);
    } else if(z == ict::L_ZONE) {
        return wxPoint2DDouble(p.m_x - getLeft(c), 0);
    } else if(z == ict::IN_ZONE) {
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

ict::RectZone CanvasItem::getZonePressed() const {
    return zonePressed;
}

ict::RectZone CanvasItem::getLocation(const wxPoint2DDouble &canvasPoint) const {
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

ict::RectZone CanvasItem::press(const wxPoint &canvasPoint) {
    if(locked) return ict::NONE_ZONE;
    zonePressed = getLocation(canvasPoint);
    if (!zonePressed) {
        selected = false;
        return zonePressed;
    }
    selected = true;
    relativePress = relativeToEdge(canvasPoint, zonePressed, CANVAS_CONTEXT);
    relativePress = scaler->scalePoint(relativePress, ict::OUT_D);
    lastPoint = relativeToEdge(canvasPoint, ict::NONE_ZONE, CANVAS_CONTEXT);
    lastPoint = scaler->scalePoint(lastPoint, ict::OUT_D);
    lastPoint -= relativePress;
    return zonePressed;
}

void CanvasItem::release() {
    zonePressed = ict::NONE_ZONE;
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
    if (!zonePressed) return false;
    lastPoint = relativeToEdge(canvasPoint, ict::NONE_ZONE, CANVAS_CONTEXT);
    lastPoint = scaler->scalePoint(lastPoint, ict::OUT_D);
    lastPoint -= relativePress;
    bool changed = geometry.pushZoneTo(zonePressed, lastPoint);
    zonePressed = geometry.getLastZone();
    if (changed) return true;
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

void CanvasItem::expandFromCenter(bool op) {
    geometry.expandFromCenter(op);
    if (!zonePressed || zonePressed == ict::IN_ZONE) return;
    if (op) geometry.useMark();
    geometry.pushZoneTo(zonePressed, lastPoint);
}

void CanvasItem::fixedAspectRatio(bool op) {
    geometry.fixedAspectRatio(op);
    if (!zonePressed || zonePressed == ict::IN_ZONE) return;
    if (op) geometry.useMark();
    geometry.pushZoneTo(zonePressed, lastPoint);
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
