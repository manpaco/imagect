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

#include "canvasitem.h"
#include "defs.h"
#include "scaler.h"
#include <wx/dcclient.h>
#include <wx/gdicmn.h>

CanvasItem::CanvasItem() {
    this->geometry = wxRect(0, 0, 1, 1);
    this->parent = nullptr;
    this->scaler = nullptr;
    this->locked = true;
    this->constraint = geometry;
    this->selected = false;
    this->fixed = false;
    this->conState = false;
    this->zPressed = ict::NONE;
    updateScaledZones();
}

CanvasItem::CanvasItem(wxRect geometry, CanvasItem *parent, Scaler *scaler, bool locked) {
    this->geometry = geometry;
    this->parent = parent;
    this->scaler = scaler;
    this->locked = locked;
    this->constraint = geometry;
    this->selected = false;
    this->fixed = false;
    this->conState = false;
    this->zPressed = ict::NONE;
    updateScaledZones();
}

CanvasItem::~CanvasItem() {

}

void CanvasItem::updateScaledZones() {
    int x1 = geometry.GetX(), x2 = geometry.GetX() + geometry.GetWidth();
    int y1 = geometry.GetY(), y2 = geometry.GetY() + geometry.GetHeight();
    int sx1 = scaler->scaleX(x1, ict::IN_D);
    int sy1 = scaler->scaleY(y1, ict::IN_D);
    int sx2 = scaler->scaleX(x2, ict::IN_D);
    int sy2 = scaler->scaleY(y2, ict::IN_D);
    wxPoint ipt(sx1, sy1);
    wxSize isz(sx2 - sx1, sy2 - sy1);
    scaledZones[ict::INNER] = wxRect(ipt, isz);
    if (locked) return;
    scaledZones[ict::N] = wxRect(sx1, sy1 - ict::CORNER, isz.GetWidth(), ict::CORNER);
    scaledZones[ict::S] = wxRect(sx1, sy2, isz.GetWidth(), ict::CORNER);
    scaledZones[ict::E] = wxRect(sx2, sy1, ict::CORNER, isz.GetHeight());
    scaledZones[ict::W] = wxRect(sx1 - ict::CORNER, sy1, ict::CORNER, isz.GetHeight());
    scaledZones[ict::NE] = wxRect(sx2, sy1 - ict::CORNER, ict::CORNER, ict::CORNER);
    scaledZones[ict::NW] = wxRect(sx1 - ict::CORNER, sy1 - ict::CORNER, ict::CORNER, ict::CORNER);
    scaledZones[ict::SE] = wxRect(sx2, sy2, ict::CORNER, ict::CORNER);
    scaledZones[ict::SW] = wxRect(sx1 - ict::CORNER, sy2, ict::CORNER, ict::CORNER);
}

wxPoint CanvasItem::relativeToEdge(const wxPoint &p, ict::ItemZone z) {
    if(z == ict::NE) {
        int t = geometry.GetY();
        int r = geometry.GetX() + geometry.GetWidth();
        return wxPoint(p.x - r, p.y - t);
    } else if(z == ict::NW) {
        int t = geometry.GetY();
        int l = geometry.GetX();
        return wxPoint(p.x - l, p.y - t);
    } else if(z == ict::SE) {
        int d = geometry.GetY() + geometry.GetHeight();
        int r = geometry.GetX() + geometry.GetWidth();
        return wxPoint(p.x - r, p.y - d);
    } else if(z == ict::SW) {
        int d = geometry.GetY() + geometry.GetHeight();
        int l = geometry.GetX();
        return wxPoint(p.x - l, p.y - d);
    } else if(z == ict::N) {
        int t = geometry.GetY();
        return wxPoint(0, p.y - t);
    } else if(z == ict::S) {
        int d = geometry.GetY() + geometry.GetHeight();
        return wxPoint(0, p.y - d);
    } else if(z == ict::E) {
        int r = geometry.GetX() + geometry.GetWidth();
        return wxPoint(p.x - r, 0);
    } else if(z == ict::W) {
        int l = geometry.GetX();
        return wxPoint(p.x - l, 0);
    } else if(z == ict::INNER)
        return wxPoint(p - geometry.GetPosition());
    return wxPoint(0, 0);
}

ict::ItemZone CanvasItem::zonePressed() const {
    return zPressed;
}

ict::ItemZone CanvasItem::getLocation(const wxPoint &p) const {
    if(scaledZones[ict::NE].Contains(p)) return ict::NE;
    if(scaledZones[ict::NW].Contains(p)) return ict::NW;
    if(scaledZones[ict::SE].Contains(p)) return ict::SE;
    if(scaledZones[ict::SW].Contains(p)) return ict::SW;
    if(scaledZones[ict::N].Contains(p)) return ict::N;
    if(scaledZones[ict::S].Contains(p)) return ict::S;
    if(scaledZones[ict::E].Contains(p)) return ict::E;
    if(scaledZones[ict::W].Contains(p)) return ict::W;
    if(scaledZones[ict::INNER].Contains(p)) return ict::INNER;
    return ict::NONE;
}

ict::ItemZone CanvasItem::press(const wxPoint &p) {
    if(locked) return ict::NONE;
    // convert p to coordinates relative to parent
    wxPoint pParent(0, 0);
    if (!isKey()) pParent = parent->getScaledPosition(false);
    wxPoint relPoint = p - pParent;
    zPressed = getLocation(relPoint);
    if (zPressed == ict::NONE) return zPressed;
    lastPoint = scaler->scalePoint(relPoint, ict::OUT_D);
    relativePress = relativeToEdge(lastPoint, zPressed);
    unmodGeo = geometry;
    return zPressed;
}

void CanvasItem::release() {
    zPressed = ict::NONE;
    if(!fixed && unmodGeo != geometry) {
        resetAccums();
    }
}

void CanvasItem::resetAccums() {
    accumX = 0.0; accumY = 0.0;
}

wxSize CanvasItem::getDimensions() const {
    return geometry.GetSize();
}

wxPoint CanvasItem::getPosition() const {
    return geometry.GetPosition();
}

wxRect CanvasItem::getGeometry() const {
    return wxRect(getPosition(), getDimensions());
}

wxSize CanvasItem::getScaledDimensions() const {
    return scaledZones[ict::INNER].GetSize();
}

wxPoint CanvasItem::getScaledPosition(const bool relativeToParent) const {
    if (!isKey() && !relativeToParent) {
        return parent->getScaledPosition(relativeToParent) + scaledZones[ict::INNER].GetPosition();
    }
    return scaledZones[ict::INNER].GetPosition();
}

wxRect CanvasItem::getScaledGeometry(const bool relativeToParent) const {
    return wxRect(getScaledPosition(relativeToParent), getScaledDimensions());
}

bool CanvasItem::isKey() const {
    return parent == nullptr;
}

bool CanvasItem::constraintOn() const {
    return conState;
}

bool CanvasItem::modify(const wxPoint &target) {
    // calcular vTarget
    // actualizar lastPoint
    if (zonePressed() == ict::NONE) return false;
    wxPoint pParent(0, 0);
    if (!isKey()) pParent = parent->getScaledPosition(false);
    wxPoint relPoint = target - pParent;
    lastPoint = scaler->scalePoint(relPoint, ict::OUT_D);
    wxRect prev = geometry;
    if (zonePressed() == ict::INNER) {
        move(lastPoint);
    } else {
        resize(lastPoint);
    }
    if (prev != geometry) {
        updateScaledZones();
        return true;
    } else return false;
}

void CanvasItem::move(const wxPoint &t) {
    wxPoint newPos(t - relativePress);
    if(newPos == geometry.GetPosition()) return;
    geometry.SetPosition(newPos);
    pushToConstraint();
}

void CanvasItem::resize(const wxPoint &target) {
    int deltaX = - relativePress.x;
    int deltaY = - relativePress.y;
    if(zonePressed() == ict::SE) {
        deltaX += target.x - (geometry.x + geometry.GetWidth());
        deltaY += target.y - (geometry.y + geometry.GetHeight());
        if(fixed) accumulateX(deltaX, deltaY);
        geometry = wxRect(geometry.x, geometry.y, geometry.GetWidth() + deltaX, 
                geometry.GetHeight() + deltaY);
    } else if(zonePressed() == ict::NW) {
        deltaX += target.x - geometry.x;
        deltaY += target.y - geometry.y;
        if(fixed) accumulateX(deltaX, deltaY);
        geometry = wxRect(geometry.x + deltaX, geometry.y + deltaY,
                geometry.GetWidth() - deltaX, 
                geometry.GetHeight() - deltaY);
    } else if(zonePressed() == ict::NE) {
        deltaX += target.x - (geometry.x + geometry.GetWidth());
        deltaY += target.y - geometry.y;
        if(fixed) { accumulateX(deltaX, deltaY); deltaX = -deltaX; }
        geometry = wxRect(geometry.x, geometry.y + deltaY, 
                geometry.GetWidth() + deltaX, 
                geometry.GetHeight() - deltaY);
    } else if(zonePressed() == ict::SW) {
        deltaX += target.x - geometry.x;
        deltaY += target.y - (geometry.y + geometry.GetHeight());
        if(fixed) { accumulateX(deltaX, deltaY); deltaX = -deltaX; }
        geometry = wxRect(geometry.x + deltaX, geometry.y, 
                geometry.GetWidth() - deltaX, 
                geometry.GetHeight() + deltaY);
    } else if(zonePressed() == ict::N) {
        deltaY += target.y - geometry.y;
        if(fixed) accumulateX(deltaX, deltaY);
        geometry = wxRect(geometry.x + deltaX, geometry.y + deltaY, 
                geometry.GetWidth() - deltaX, 
                geometry.GetHeight() - deltaY);
    } else if(zonePressed() == ict::S) {
        deltaY += target.y - (geometry.y + geometry.GetHeight());
        if(fixed) accumulateX(deltaX, deltaY);
        geometry = wxRect(geometry.x, geometry.y, 
                geometry.GetWidth() + deltaX, 
                geometry.GetHeight() + deltaY);
    } else if(zonePressed() == ict::W) {
        deltaX += target.x - geometry.x;
        if(fixed) { accumulateY(deltaY, deltaX); deltaY = -deltaY; }
        geometry = wxRect(geometry.x + deltaX, geometry.y, 
                geometry.GetWidth() - deltaX, 
                geometry.GetHeight() + deltaY);
    } else if(zonePressed() == ict::E) {
        deltaX += target.x - (geometry.x + geometry.GetWidth());
        if(fixed) { accumulateY(deltaY, deltaX); deltaY = -deltaY; }
        geometry = wxRect(geometry.x, geometry.y + deltaY, 
                geometry.GetWidth() + deltaX, 
                geometry.GetHeight() - deltaY);
    }
    fitInConstraint();
}

void CanvasItem::pushToConstraint() {
    if(!constraintOn() || constraint.Contains(geometry)) return;
    int cx1 = constraint.GetX(), cx2 = constraint.GetX() + constraint.GetWidth();
    int nx1 = geometry.GetX(), nx2 = geometry.GetX() + geometry.GetWidth();
    if(nx2 > cx2)
        geometry.SetPosition(wxPoint(cx2 - geometry.GetWidth(), geometry.GetY()));
    if(nx1 < cx1) geometry.SetPosition(wxPoint(cx1, geometry.GetY()));
    int cy1 = constraint.GetY(), cy2 = constraint.GetY() + constraint.GetHeight();
    int ny1 = geometry.GetY(), ny2 = geometry.GetY() + geometry.GetHeight();
    if(ny2 > cy2)
        geometry.SetPosition(wxPoint(geometry.GetX(), cy2 - geometry.GetHeight()));
    if(ny1 < cy1) geometry.SetPosition(wxPoint(geometry.GetX(), cy1));
}

void CanvasItem::fitInConstraint() {
    if(!constraintOn() || constraint.Contains(geometry)) return;
    if(!fixed) {
        geometry.Intersect(constraint);
        return;
    }
    bool exceedsTopY = geometry.GetY() < constraint.GetY();
    bool exceedsBottomY =
        (geometry.GetY() + geometry.GetHeight()) > (constraint.GetY() + constraint.GetHeight());
    bool exceedsLeftX = geometry.GetX() < constraint.GetX();
    bool exceedsRightX =
        (geometry.GetX() + geometry.GetWidth()) > (constraint.GetX() + constraint.GetWidth());
    wxRect aux(geometry);
    aux.Intersect(constraint);
    int newWidth, newHeight;
    if(zPressed == ict::SE || zPressed == ict::S) {
        if(exceedsRightX) {
            newWidth = aux.GetWidth();
            accumulateY(newHeight, newWidth);
            geometry.SetSize(wxSize(newWidth, newHeight));
            exceedsBottomY =
                (geometry.GetY() + geometry.GetHeight()) >
                (constraint.GetY() + constraint.GetHeight());
        }
        if(exceedsBottomY) {
            newHeight = aux.GetHeight();
            accumulateX(newWidth, newHeight);
            geometry.SetSize(wxSize(newWidth, newHeight));
        }
    } else if(zPressed == ict::SW || zPressed == ict::W) {
        if(exceedsLeftX) {
            newWidth = aux.GetWidth();
            accumulateY(newHeight, newWidth);
            geometry.SetSize(wxSize(newWidth, newHeight));
            geometry.SetPosition(wxPoint(constraint.GetX(), geometry.GetY()));
            exceedsBottomY =
                (geometry.GetY() + geometry.GetHeight()) >
                (constraint.GetY() + constraint.GetHeight());
        }
        if(exceedsBottomY) {
            newHeight = aux.GetHeight();
            accumulateX(newWidth, newHeight);
            wxPoint newPos(geometry.GetX() + geometry.GetWidth() - newWidth,
                           geometry.GetY());
            geometry.SetSize(wxSize(newWidth, newHeight));
            geometry.SetPosition(newPos);
        }
    } else if(zPressed == ict::NE || zPressed == ict::E) {
        if(exceedsRightX) {
            newWidth = aux.GetWidth();
            accumulateY(newHeight, newWidth);
            wxPoint newPos(geometry.GetX(),
                           geometry.GetY() + geometry.GetHeight() - newHeight);
            geometry.SetSize(wxSize(newWidth, newHeight));
            geometry.SetPosition(newPos);
            exceedsTopY = geometry.GetY() < constraint.GetY();
        }
        if(exceedsTopY) {
            newHeight = aux.GetHeight();
            accumulateX(newWidth, newHeight);
            geometry.SetSize(wxSize(newWidth, newHeight));
            geometry.SetPosition(wxPoint(geometry.GetX(), constraint.GetY()));
        }
    } else if(zPressed == ict::NW || zPressed == ict::N) {
        if(exceedsLeftX) {
            newWidth = aux.GetWidth();
            accumulateY(newHeight, newWidth);
            wxPoint newPos(constraint.GetX(),
                           geometry.GetY() + geometry.GetHeight() - newHeight);
            geometry.SetSize(wxSize(newWidth, newHeight));
            geometry.SetPosition(newPos);
            exceedsTopY = geometry.GetY() < constraint.GetY();
        }
        if(exceedsTopY) {
            newHeight = aux.GetHeight();
            accumulateX(newWidth, newHeight);
            wxPoint newPos(geometry.GetX() + geometry.GetWidth() - newWidth,
                           constraint.GetY());
            geometry.SetSize(wxSize(newWidth, newHeight));
            geometry.SetPosition(newPos);
        }
    }
}

void CanvasItem::setConstraint(const wxRect &constraint) {
    this->constraint = constraint;
}

bool CanvasItem::constraintState(bool op) {
    conState = op;
    if(op) return setGeometry(geometry);
    else return false;
}

void CanvasItem::accumulateX(int &dxToCalc, int &dyToUse) {
    accumX += (double)dyToUse * unmodRatio();
    dxToCalc = std::floor(accumX);
    accumX -= dxToCalc;
}

void CanvasItem::accumulateY(int &dyToCalc, int &dxToUse) {
    accumY += (double)dxToUse / unmodRatio();
    dyToCalc = std::floor(accumY);
    accumY -= dyToCalc;
}

void CanvasItem::fixRatio(bool op) {
    if (zPressed == ict::NONE || zPressed == ict::INNER) return;
    fixed = op;
    if (fixed) {
        geometry = unmodGeo;
    }
    modify(lastPoint);
}

bool CanvasItem::setDimensions(const wxSize &dim) {
    return setGeometry(wxRect(getPosition(), dim));
}

bool CanvasItem::setGeometry(const wxRect &geo) {
    if (geometry == geo) return false;
    wxRect prev = geometry;
    geometry = geo;
    if (constraintOn()) {
        if (!constraint.Contains(geometry.GetPosition())) {
            geometry.SetPosition(constraint.GetPosition());
        }
        geometry.Intersect(constraint);
    }
    if (prev != geometry) {
        updateScaledZones();
        return true;
    } else return false;
}

bool CanvasItem::setPosition(const wxPoint &pos) {
    return setGeometry(wxRect(pos, geometry.GetSize()));
}

void CanvasItem::doMagnify(wxPoint magCenter) {
    if (isKey()) {
        // center magnify
        wxPoint magPos = scaledZones[ict::INNER].GetPosition();
        magPos -= magCenter;
        magPos = scaler->transferPoint(magPos, ict::IN_D);
        magPos += magCenter;
        geometry.SetPosition(scaler->scalePoint(magPos, ict::OUT_D));
    }
    updateScaledZones();
}

void CanvasItem::doScroll(wxPoint motion) {
    if (!isKey()) return;
    geometry.SetPosition(geometry.GetPosition() + motion);
    updateScaledZones();
}

double CanvasItem::unmodRatio() const {
    return (double)unmodGeo.GetWidth() / unmodGeo.GetHeight();
}

void CanvasItem::drawOn(wxPaintDC *pv) {
    pv->SetBrush(*wxRED_BRUSH);
    pv->DrawRectangle(getScaledGeometry(false));
}
