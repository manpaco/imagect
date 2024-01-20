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

CanvasItem::CanvasItem() : CanvasItem(-1, wxRect(0, 0, 1, 1)) {

}

CanvasItem::CanvasItem(int id, wxRect geometry) {
    this->id = id;
    this->geometry = geometry;
    this->scaler = nullptr;
    this->locked = true;
    this->restriction = geometry;
    this->selected = false;
    this->fixed = false;
    this->restricted = false;
    this->zonePressed = ict::NONE;
    this->offset = wxPoint(0, 0);;
    this->reference = nullptr;
}

CanvasItem::~CanvasItem() {

}

void CanvasItem::setCanvasReference(CanvasItem *r) {
    reference = r;
}

int CanvasItem::yVirtualUnref() const {
    return geometry.GetY();
}

int CanvasItem::xVirtualUnref() const {
    return geometry.GetX();
}

int CanvasItem::getX(ItemContext ic) const {
    if (ic == VIRTUAL_CONTEXT) {
        int rx = 0;
        if (reference) rx = reference->getX(ic);
        return geometry.GetX() + rx;
    }
    else return scaler->scaleX(getX(VIRTUAL_CONTEXT), ict::IN_D);
}

int CanvasItem::getY(ItemContext ic) const {
    int ry = 0;
    if (ic == VIRTUAL_CONTEXT) {
        if (reference) ry = reference->getY(ic);
        return geometry.GetY() + ry;
    }
    else return scaler->scaleY(getY(VIRTUAL_CONTEXT), ict::IN_D);
}

int CanvasItem::getWidth(ItemContext ic) const {
    if (ic == VIRTUAL_CONTEXT) return geometry.GetWidth();
    else return scaler->scaleX(getRight(VIRTUAL_CONTEXT) - getLeft(VIRTUAL_CONTEXT), ict::IN_D);
}

int CanvasItem::getHeight(ItemContext ic) const {
    if (ic == VIRTUAL_CONTEXT) return geometry.GetHeight();
    else return scaler->scaleY(getBottom(VIRTUAL_CONTEXT) - getTop(VIRTUAL_CONTEXT), ict::IN_D);
}

int CanvasItem:: getRight(ItemContext ic) const {
    return getLeft(ic) + getWidth(ic);
}

int CanvasItem::getBottom(ItemContext ic) const {
    return getTop(ic) + getHeight(ic);
}

int CanvasItem::getLeft(ItemContext ic) const {
    return getX(ic);
}

int CanvasItem::getTop(ItemContext ic) const {
    return getY(ic);
}

wxRect CanvasItem::getZone(ict::ItemZone z) const {
    if(z == ict::NE) {
        return wxRect(getRight(CANVAS_CONTEXT), getTop(CANVAS_CONTEXT) - ict::CORNER, ict::CORNER, ict::CORNER);
    } else if(z == ict::NW) {
        return wxRect(getLeft(CANVAS_CONTEXT) - ict::CORNER, getTop(CANVAS_CONTEXT) - ict::CORNER, ict::CORNER, ict::CORNER);
    } else if(z == ict::SE) {
        return wxRect(getRight(CANVAS_CONTEXT), getBottom(CANVAS_CONTEXT), ict::CORNER, ict::CORNER);
    } else if(z == ict::SW) {
        return wxRect(getLeft(CANVAS_CONTEXT) - ict::CORNER, getBottom(CANVAS_CONTEXT), ict::CORNER, ict::CORNER);
    } else if(z == ict::N) {
        return wxRect(getLeft(CANVAS_CONTEXT), getTop(CANVAS_CONTEXT) - ict::CORNER, getWidth(CANVAS_CONTEXT), ict::CORNER);
    } else if(z == ict::S) {
        return wxRect(getLeft(CANVAS_CONTEXT), getBottom(CANVAS_CONTEXT), getWidth(CANVAS_CONTEXT), ict::CORNER);
    } else if(z == ict::E) {
        return wxRect(getRight(CANVAS_CONTEXT), getTop(CANVAS_CONTEXT), ict::CORNER, getHeight(CANVAS_CONTEXT));
    } else if(z == ict::W) {
        return wxRect(getLeft(CANVAS_CONTEXT) - ict::CORNER, getTop(CANVAS_CONTEXT), ict::CORNER, getHeight(CANVAS_CONTEXT));
    } else if(z == ict::INNER)
        return getGeometry(CANVAS_CONTEXT);
    return wxRect(0, 0, 0, 0);
}

wxPoint CanvasItem::relativeToEdge(const wxPoint &p, ict::ItemZone z) {
    if(z == ict::NE) {
        return wxPoint(p.x - getRight(CANVAS_CONTEXT), p.y - getTop(CANVAS_CONTEXT));
    } else if(z == ict::NW) {
        return wxPoint(p.x - getLeft(CANVAS_CONTEXT), p.y - getTop(CANVAS_CONTEXT));
    } else if(z == ict::SE) {
        return wxPoint(p.x - getRight(CANVAS_CONTEXT), p.y - getBottom(CANVAS_CONTEXT));
    } else if(z == ict::SW) {
        return wxPoint(p.x - getLeft(CANVAS_CONTEXT), p.y - getBottom(CANVAS_CONTEXT));
    } else if(z == ict::N) {
        return wxPoint(0, p.y - getTop(CANVAS_CONTEXT));
    } else if(z == ict::S) {
        return wxPoint(0, p.y - getBottom(CANVAS_CONTEXT));
    } else if(z == ict::E) {
        return wxPoint(p.x - getRight(CANVAS_CONTEXT), 0);
    } else if(z == ict::W) {
        return wxPoint(p.x - getLeft(CANVAS_CONTEXT), 0);
    } else if(z == ict::INNER)
        return wxPoint(p - getPosition(CANVAS_CONTEXT));
    return wxPoint(0, 0);
}

ict::ItemZone CanvasItem::getZonePressed() const {
    return zonePressed;
}

ict::ItemZone CanvasItem::getLocation(const wxPoint &canvasPoint) const {
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
    lastPoint = canvasPoint;
    if (reference) lastPoint = reference->relativeToEdge(canvasPoint, ict::INNER);
    lastPoint = scaler->scalePoint(lastPoint, ict::OUT_D);
    relativePress = relativeToEdge(canvasPoint, zonePressed);
    relativePress = scaler->scalePoint(relativePress, ict::OUT_D);
    return zonePressed;
}

void CanvasItem::release() {
    zonePressed = ict::NONE;
    unmodGeometry = geometry;
    if(!fixed && unmodGeometry != geometry) {
        resetAccums();
    }
}

void CanvasItem::resetAccums() {
    accumX = 0.0; accumY = 0.0;
}

wxSize CanvasItem::getDimensions(ItemContext ic) const {
    return wxSize(getWidth(ic), getHeight(ic));
}

wxPoint CanvasItem::getPosition(ItemContext ic) const {
    return wxPoint(getLeft(ic), getTop(ic));
}

wxRect CanvasItem::getGeometry(ItemContext ic) const {
    return wxRect(getPosition(ic), getDimensions(ic));
}

wxRect CanvasItem::getArea() const {
    return getGeometry(CANVAS_CONTEXT).Inflate(ict::CORNER, ict::CORNER);
}

bool CanvasItem::isRestricted() const {
    return restricted;
}

bool CanvasItem::modify(const wxPoint &canvasPoint) {
    if (zonePressed == ict::NONE) return false;
    lastPoint = canvasPoint;
    if (reference) lastPoint = reference->relativeToEdge(canvasPoint, ict::INNER);
    lastPoint = scaler->scalePoint(lastPoint, ict::OUT_D);
    wxRect prev = geometry;
    if (zonePressed == ict::INNER) {
        move(lastPoint);
    } else {
        resize(lastPoint);
    }
    if (prev != geometry) return true;
    else return false;
}

void CanvasItem::move(const wxPoint &target) {
    wxPoint newPos(target - relativePress);
    if(newPos == geometry.GetPosition()) return;
    geometry.SetPosition(newPos);
    pushToRestriction();
}

void CanvasItem::resize(const wxPoint &target) {
    int deltaX = - relativePress.x;
    int deltaY = - relativePress.y;
    if(zonePressed == ict::SE) {
        deltaX += target.x - (geometry.x + geometry.GetWidth());
        deltaY += target.y - (geometry.y + geometry.GetHeight());
        if(fixed) accumulateX(deltaX, deltaY);
        geometry = wxRect(geometry.x, geometry.y, geometry.GetWidth() + deltaX,
                geometry.GetHeight() + deltaY);
    } else if(zonePressed == ict::NW) {
        deltaX += target.x - geometry.x;
        deltaY += target.y - geometry.y;
        if(fixed) accumulateX(deltaX, deltaY);
        geometry = wxRect(geometry.x + deltaX, geometry.y + deltaY,
                geometry.GetWidth() - deltaX,
                geometry.GetHeight() - deltaY);
    } else if(zonePressed == ict::NE) {
        deltaX += target.x - (geometry.x + geometry.GetWidth());
        deltaY += target.y - geometry.y;
        if(fixed) { accumulateX(deltaX, deltaY); deltaX = -deltaX; }
        geometry = wxRect(geometry.x, geometry.y + deltaY,
                geometry.GetWidth() + deltaX,
                geometry.GetHeight() - deltaY);
    } else if(zonePressed == ict::SW) {
        deltaX += target.x - geometry.x;
        deltaY += target.y - (geometry.y + geometry.GetHeight());
        if(fixed) { accumulateX(deltaX, deltaY); deltaX = -deltaX; }
        geometry = wxRect(geometry.x + deltaX, geometry.y,
                geometry.GetWidth() - deltaX,
                geometry.GetHeight() + deltaY);
    } else if(zonePressed == ict::N) {
        deltaY += target.y - geometry.y;
        if(fixed) accumulateX(deltaX, deltaY);
        geometry = wxRect(geometry.x + deltaX, geometry.y + deltaY,
                geometry.GetWidth() - deltaX,
                geometry.GetHeight() - deltaY);
    } else if(zonePressed == ict::S) {
        deltaY += target.y - (geometry.y + geometry.GetHeight());
        if(fixed) accumulateX(deltaX, deltaY);
        geometry = wxRect(geometry.x, geometry.y,
                geometry.GetWidth() + deltaX,
                geometry.GetHeight() + deltaY);
    } else if(zonePressed == ict::W) {
        deltaX += target.x - geometry.x;
        if(fixed) { accumulateY(deltaY, deltaX); deltaY = -deltaY; }
        geometry = wxRect(geometry.x + deltaX, geometry.y,
                geometry.GetWidth() - deltaX,
                geometry.GetHeight() + deltaY);
    } else if(zonePressed == ict::E) {
        deltaX += target.x - (geometry.x + geometry.GetWidth());
        if(fixed) { accumulateY(deltaY, deltaX); deltaY = -deltaY; }
        geometry = wxRect(geometry.x, geometry.y + deltaY,
                geometry.GetWidth() + deltaX,
                geometry.GetHeight() - deltaY);
    }
    fitInRestriction(zonePressed);
}

void CanvasItem::pushToRestriction() {
    if(!isRestricted() || restriction.Contains(geometry)) return;
    int cx1 = restriction.GetX(), cx2 = restriction.GetX() + restriction.GetWidth();
    int nx1 = geometry.GetX(), nx2 = geometry.GetX() + geometry.GetWidth();
    if(nx2 > cx2)
        geometry.SetPosition(wxPoint(cx2 - geometry.GetWidth(), geometry.GetY()));
    if(nx1 < cx1) geometry.SetPosition(wxPoint(cx1, geometry.GetY()));
    int cy1 = restriction.GetY(), cy2 = restriction.GetY() + restriction.GetHeight();
    int ny1 = geometry.GetY(), ny2 = geometry.GetY() + geometry.GetHeight();
    if(ny2 > cy2)
        geometry.SetPosition(wxPoint(geometry.GetX(), cy2 - geometry.GetHeight()));
    if(ny1 < cy1) geometry.SetPosition(wxPoint(geometry.GetX(), cy1));
}

void CanvasItem::fitInRestriction(ict::ItemZone simulation) {
    if(!isRestricted() || restriction.Contains(geometry)) return;
    if(!fixed) {
        geometry.Intersect(restriction);
        return;
    }
    bool exceedsTopY = geometry.GetY() < restriction.GetY();
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
    }
}

void CanvasItem::setVirtualRestriction(const wxRect &restriction) {
    this->restriction = restriction;
}

bool CanvasItem::restrict(bool opt) {
    restricted = opt;
    if(opt) return applyGeometry(geometry);
    else return false;
}

void CanvasItem::accumulateX(int &dxToCalc, int &dyToUse) {
    accumX += (double)dyToUse * getUnmodAspectRatio();
    dxToCalc = std::floor(accumX);
    accumX -= dxToCalc;
}

void CanvasItem::accumulateY(int &dyToCalc, int &dxToUse) {
    accumY += (double)dxToUse / getUnmodAspectRatio();
    dyToCalc = std::floor(accumY);
    accumY -= dyToCalc;
}

void CanvasItem::setAspectRatio(int xr, int yr) {

}

void CanvasItem::fixAspectRatio(bool op) {
    fixed = op;
    if (zonePressed == ict::NONE || zonePressed == ict::INNER) return;
    if (fixed) geometry = unmodGeometry;
    resize(lastPoint);
}

bool CanvasItem::setVirtualDimensions(const wxSize &dim) {
    return setVirtualGeometry(wxRect(getPosition(VIRTUAL_CONTEXT), dim));
}

bool CanvasItem::applyGeometry(const wxRect &geo) {
    wxRect prev = geometry;
    geometry = geo;
    unmodGeometry = geometry;
    if (isRestricted()) {
        if (!restriction.Contains(geometry.GetPosition())) {
            geometry.SetPosition(restriction.GetPosition());
        }
        fitInRestriction(ict::SE);
    }
    if (prev != geometry) return true;
    else return false;
}

bool CanvasItem::setVirtualGeometry(const wxRect &geo) {
    if (geometry == geo) return false;
    if (geometry.GetSize() != geo.GetSize()) resetAccums();
    return applyGeometry(geo);
}

bool CanvasItem::setVirtualPosition(const wxPoint &pos) {
    return setVirtualGeometry(wxRect(pos, getDimensions(VIRTUAL_CONTEXT)));
}

double CanvasItem::getUnmodAspectRatio() const {
    return (double)unmodGeometry.GetWidth() / unmodGeometry.GetHeight();
}

double CanvasItem::getAspectRatio() const {
    return (double)geometry.GetWidth() / geometry.GetHeight();
}

wxPoint CanvasItem::getOffset() const {
    return offset;
}

void CanvasItem::setOffset(wxPoint vo) {
    offset = vo;
}

void CanvasItem::drawOn(wxMemoryDC *pv) {
    drawEntries(pv);
    pv->SetBrush(*wxRED_BRUSH);
    pv->DrawRectangle(getGeometry(CANVAS_CONTEXT));
}

void CanvasItem::drawEntries(wxMemoryDC *pv) {
    if (!selected) return;
    pv->SetBrush(*wxBLUE_BRUSH);
    pv->DrawRectangle(getArea());
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
