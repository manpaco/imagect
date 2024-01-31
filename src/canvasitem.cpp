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
    this->restriction = geometry;
    this->selected = false;
    this->fixed = false;
    this->restricted = false;
    this->zonePressed = ict::NONE;
    this->reference = nullptr;
}

CanvasItem::~CanvasItem() {

}

void CanvasItem::setVirtualReference(wxPoint2DDouble *r) {
    reference = r;
}

double CanvasItem::getX(ItemContext ic, bool unref) const {
    if (ic == VIRTUAL_CONTEXT) {
        double rx = 0;
        if (reference && !unref) rx = reference->m_x;
        return geometry.m_x + rx;
    }
    else return scaler->scaleX(getX(VIRTUAL_CONTEXT, unref), ict::IN_D);
}

double CanvasItem::getY(ItemContext ic, bool unref) const {
    if (ic == VIRTUAL_CONTEXT) {
        double ry = 0;
        if (reference && !unref) ry = reference->m_y;
        return geometry.m_y + ry;
    }
    else return scaler->scaleY(getY(VIRTUAL_CONTEXT, unref), ict::IN_D);
}

double CanvasItem::getWidth(ItemContext ic) const {
    if (ic == VIRTUAL_CONTEXT) return geometry.m_width;
    else return scaler->scaleX(geometry.m_width, ict::IN_D);
}

double CanvasItem::getHeight(ItemContext ic) const {
    if (ic == VIRTUAL_CONTEXT) return geometry.m_height;
    else return scaler->scaleY(geometry.m_height, ict::IN_D);
}

double CanvasItem:: getRight(ItemContext ic, bool unref) const {
    return getLeft(ic, unref) + getWidth(ic);
}

double CanvasItem::getBottom(ItemContext ic, bool unref) const {
    return getTop(ic, unref) + getHeight(ic);
}

double CanvasItem::getLeft(ItemContext ic, bool unref) const {
    return getX(ic, unref);
}

double CanvasItem::getTop(ItemContext ic, bool unref) const {
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
    // convert p to coordinates relative to parent
    zonePressed = getLocation(canvasPoint);
    if (zonePressed == ict::NONE) {
        selected = false;
        return zonePressed;
    }
    selected = true;
    lastPoint = relativeToEdge(canvasPoint, ict::NONE, CANVAS_CONTEXT);
    lastPoint = scaler->scalePoint(lastPoint, ict::OUT_D);
    relativePress = relativeToEdge(canvasPoint, zonePressed, CANVAS_CONTEXT);
    relativePress = scaler->scalePoint(relativePress, ict::OUT_D);
    return zonePressed;
}

void CanvasItem::release() {
    zonePressed = ict::NONE;
    unmodGeometry = geometry;
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
    return getGeometry(CANVAS_CONTEXT);
}

bool CanvasItem::isRestricted() const {
    return restricted;
}

bool CanvasItem::modify(const wxPoint &canvasPoint) {
    if (zonePressed == ict::NONE) return false;
    lastPoint = relativeToEdge(canvasPoint, ict::NONE, CANVAS_CONTEXT);
    lastPoint = scaler->scalePoint(lastPoint, ict::OUT_D);
    wxRect2DDouble prev = geometry;
    if (zonePressed == ict::INNER) {
        move(lastPoint);
    } else {
        resize(lastPoint);
    }
    if (prev != geometry) return true;
    else return false;
}

void CanvasItem::move(const wxPoint2DDouble &target) {
    wxPoint2DDouble newPos(target - relativePress);
    if(newPos == geometry.GetPosition()) return;
    geometry.m_x = newPos.m_x;
    geometry.m_y = newPos.m_y;
    pushToRestriction();
}

void CanvasItem::resize(const wxPoint2DDouble &target) {
    int deltaX = - relativePress.m_x;
    int deltaY = - relativePress.m_y;
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
    fitInRestriction(zonePressed);
}

void CanvasItem::pushToRestriction() {
    if(!isRestricted() || restriction.Contains(geometry)) return;
    int cx1 = restriction.GetLeft(), cx2 = restriction.GetRight();
    int nx1 = geometry.GetLeft(), nx2 = geometry.GetRight();
    /* if(nx2 > cx2)
        geometry.SetPosition(wxPoint(cx2 - geometry.GetWidth(), geometry.GetY()));
    if(nx1 < cx1) geometry.SetPosition(wxPoint(cx1, geometry.GetY()));
    int cy1 = restriction.GetY(), cy2 = restriction.GetY() + restriction.GetHeight();
    int ny1 = geometry.GetY(), ny2 = geometry.GetY() + geometry.GetHeight();
    if(ny2 > cy2)
        geometry.SetPosition(wxPoint(geometry.GetX(), cy2 - geometry.GetHeight()));
    if(ny1 < cy1) geometry.SetPosition(wxPoint(geometry.GetX(), cy1)); */
}

void CanvasItem::fitInRestriction(ict::ItemZone simulation) {
    if(!isRestricted() || restriction.Contains(geometry)) return;
    if(!fixed) {
        geometry.Intersect(restriction);
        return;
    }
    /* bool exceedsTopY = geometry.GetY() < restriction.GetY();
    bool exceedsBottomY =
        (geometry.GetY() + geometry.GetHeight()) > (restriction.GetY() + restriction.GetHeight());
    bool exceedsLeftX = geometry.GetX() < restriction.GetX();
    bool exceedsRightX =
        (geometry.GetX() + geometry.GetWidth()) > (restriction.GetX() + restriction.GetWidth());
    wxRect aux(geometry);
    aux.Intersect(restriction);
    int newWidth, newHeight;
    if(simulation == ict::SE || simulation == ict::S) {
        if(exceedsRightX) {
            newWidth = aux.GetWidth();
            accumulateY(newHeight, newWidth);
            geometry.SetSize(wxSize(newWidth, newHeight));
            exceedsBottomY =
                (geometry.GetY() + geometry.GetHeight()) >
                (restriction.GetY() + restriction.GetHeight());
        }
        if(exceedsBottomY) {
            newHeight = aux.GetHeight();
            accumulateX(newWidth, newHeight);
            geometry.SetSize(wxSize(newWidth, newHeight));
        }
    } else if(simulation == ict::SW || simulation == ict::W) {
        if(exceedsLeftX) {
            newWidth = aux.GetWidth();
            accumulateY(newHeight, newWidth);
            geometry.SetSize(wxSize(newWidth, newHeight));
            geometry.SetPosition(wxPoint(restriction.GetX(), geometry.GetY()));
            exceedsBottomY =
                (geometry.GetY() + geometry.GetHeight()) >
                (restriction.GetY() + restriction.GetHeight());
        }
        if(exceedsBottomY) {
            newHeight = aux.GetHeight();
            accumulateX(newWidth, newHeight);
            wxPoint newPos(geometry.GetX() + geometry.GetWidth() - newWidth,
                           geometry.GetY());
            geometry.SetSize(wxSize(newWidth, newHeight));
            geometry.SetPosition(newPos);
        }
    } else if(simulation == ict::NE || simulation == ict::E) {
        if(exceedsRightX) {
            newWidth = aux.GetWidth();
            accumulateY(newHeight, newWidth);
            wxPoint newPos(geometry.GetX(),
                           geometry.GetY() + geometry.GetHeight() - newHeight);
            geometry.SetSize(wxSize(newWidth, newHeight));
            geometry.SetPosition(newPos);
            exceedsTopY = geometry.GetY() < restriction.GetY();
        }
        if(exceedsTopY) {
            newHeight = aux.GetHeight();
            accumulateX(newWidth, newHeight);
            geometry.SetSize(wxSize(newWidth, newHeight));
            geometry.SetPosition(wxPoint(geometry.GetX(), restriction.GetY()));
        }
    } else if(simulation == ict::NW || simulation == ict::N) {
        if(exceedsLeftX) {
            newWidth = aux.GetWidth();
            accumulateY(newHeight, newWidth);
            wxPoint newPos(restriction.GetX(),
                           geometry.GetY() + geometry.GetHeight() - newHeight);
            geometry.SetSize(wxSize(newWidth, newHeight));
            geometry.SetPosition(newPos);
            exceedsTopY = geometry.GetY() < restriction.GetY();
        }
        if(exceedsTopY) {
            newHeight = aux.GetHeight();
            accumulateX(newWidth, newHeight);
            wxPoint newPos(geometry.GetX() + geometry.GetWidth() - newWidth,
                           restriction.GetY());
            geometry.SetSize(wxSize(newWidth, newHeight));
            geometry.SetPosition(newPos);
        }
    } */
}

void CanvasItem::setVirtualRestriction(const wxRect2DDouble &restriction) {
    this->restriction = restriction;
}

bool CanvasItem::restrict(bool opt) {
    restricted = opt;
    if(opt) return applyGeometry(geometry);
    else return false;
}

void CanvasItem::setAspectRatio(int xr, int yr) {

}

void CanvasItem::fixAspectRatio(bool op) {
    fixed = op;
    if (zonePressed == ict::NONE || zonePressed == ict::INNER) return;
    if (fixed) geometry = unmodGeometry;
    resize(lastPoint);
}

bool CanvasItem::setVirtualDimensions(const wxPoint2DDouble &dim) {
    wxPoint2DDouble p(getPosition(VIRTUAL_CONTEXT, true));
    return setVirtualGeometry(wxRect2DDouble(p.m_x, p.m_y, dim.m_x, dim.m_y));
}

bool CanvasItem::applyGeometry(const wxRect2DDouble &geo) {
    wxRect2DDouble prev = geometry;
    geometry = geo;
    unmodGeometry = geometry;
    if (isRestricted()) {
        if (!restriction.Contains(geometry.GetPosition())) {
            // geometry.SetPosition(restriction.GetPosition());
        }
        fitInRestriction(ict::SE);
    }
    if (prev != geometry) return true;
    else return false;
}

bool CanvasItem::setVirtualGeometry(const wxRect2DDouble &geo) {
    if (geometry == geo) return false;
    return applyGeometry(geo);
}

bool CanvasItem::setVirtualPosition(const wxPoint2DDouble &pos) {
    wxPoint2DDouble d(getDimensions(VIRTUAL_CONTEXT));
    return setVirtualGeometry(wxRect2DDouble(pos.m_x, pos.m_y, d.m_x, d.m_y));
}

double CanvasItem::getUnmodAspectRatio() const {
    return unmodGeometry.m_width / unmodGeometry.m_height;
}

double CanvasItem::getAspectRatio() const {
    return geometry.m_width / geometry.m_height;
}

void CanvasItem::drawOn(wxMemoryDC *pv) {
    // drawEntries(pv);
    pv->SetBrush(*wxRED_BRUSH);
    wxRect2DDouble ddr(getGeometry(CANVAS_CONTEXT));
    wxRect dr(ddr.m_x, ddr.m_y, ddr.m_width, ddr.m_height);
    pv->DrawRectangle(dr);
}

void CanvasItem::drawEntries(wxMemoryDC *pv) {
    if (!selected) return;
    pv->SetBrush(*wxBLUE_BRUSH);
    // pv->DrawRectangle(getArea());
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
