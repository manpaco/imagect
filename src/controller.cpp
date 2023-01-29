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

#include "controller.h"
#include "defs.h"
#include <iostream>

CropController::CropController() {

}

CropController::CropController(const wxRect &r) {
    crop = r;
    cons = r;
    pressZone = ict::NONE;
    updateZones();
}

CropController::~CropController() {

}

void CropController::updateZones() {
    viz = crop;
    int x1 = viz.GetX(), x2 = viz.GetX() + viz.GetWidth();
    int y1 = viz.GetY(), y2 = viz.GetY() + viz.GetHeight();
    vnz = wxRect(x1, y1, viz.GetWidth(), ict::CORNER);
    vsz = wxRect(x1, y2 - ict::CORNER, viz.GetWidth(), ict::CORNER);
    vez = wxRect(x2 - ict::CORNER, y1, ict::CORNER, viz.GetHeight());
    vwz = wxRect(x1, y1, ict::CORNER, viz.GetHeight());
    vnez = wxRect(x2 - ict::CORNER, y1, ict::CORNER, ict::CORNER);
    vnwz = wxRect(x1, y1, ict::CORNER, ict::CORNER);
    vsez = wxRect(x2 - ict::CORNER, y2 - ict::CORNER, ict::CORNER,
                  ict::CORNER);
    vswz = wxRect(x1, y2 - ict::CORNER, ict::CORNER, ict::CORNER);
}

wxPoint CropController::relativeToZone(const wxPoint &p, ict::Zone z) {
    if(z == ict::NE) {
        int t = vnez.GetY();
        int r = vnez.GetX() + vnez.GetWidth();
        return wxPoint(p.x - r, p.y - t);
    } else if(z == ict::NW) {
        int t = vnwz.GetY();
        int l = vnwz.GetX();
        return wxPoint(p.x - l, p.y - t);
    } else if(z == ict::SE) {
        int d = vsez.GetY() + vsez.GetHeight();
        int r = vsez.GetX() + vsez.GetWidth();
        return wxPoint(p.x - r, p.y - d);
    } else if(z == ict::SW) {
        int d = vswz.GetY() + vswz.GetHeight();
        int l = vswz.GetX();
        return wxPoint(p.x - l, p.y - d);
    } else if(z == ict::N) {
        int t = vnz.GetY();
        return wxPoint(0, p.y - t);
    } else if(z == ict::S) {
        int d = vsz.GetY() + vsz.GetHeight();
        return wxPoint(0, p.y - d);
    } else if(z == ict::E) {
        int r = vez.GetX() + vez.GetWidth();
        return wxPoint(p.x - r, 0);
    } else if(z == ict::W) {
        int l = vwz.GetX();
        return wxPoint(p.x - l, 0);
    } else if(z == ict::INNER)
        return wxPoint(p - viz.GetPosition());
    return wxPoint(0, 0);
}

ict::Zone CropController::zonePressed() const {
    return pressZone;
}

void CropController::press(const ict::Zone z, const wxPoint &p) {
    pressZone = z;
    relativePress = relativeToZone(p, z);
}

void CropController::release() {
    pressZone = ict::NONE;
}

wxSize CropController::cropSize() const {
    return crop.GetSize();
}

wxPoint CropController::cropPosition() const {
    return crop.GetPosition();
}

wxRect CropController::cropRect() const {
    return crop;
}

bool CropController::constraint() const {
    return activeCons;
}

bool CropController::modify(const wxPoint &target) {
    if(!zonePressed()) return false;
    wxRect prevCrop(crop);
    if(zonePressed() == ict::INNER) {
        move(target);
    } else {
        resize(target);
    }
    if(prevCrop != crop) {
        updateZones();
        return true;
    } else return false;
}

void CropController::move(const wxPoint &t) {
    wxPoint newPos(t - relativePress);
    if(newPos == crop.GetPosition()) return;
    crop.SetPosition(newPos);
    pushToConstraint();
}

void CropController::resize(const wxPoint &target) {
    if(fix) fixHint = zonePressed();
    int deltaX = 0, deltaY = 0;
    if(zonePressed() == ict::SE) {
        deltaX += - relativePress.x;
        deltaY += - relativePress.y;
        deltaX += target.x - (crop.x + crop.GetWidth());
        deltaY += target.y - (crop.y + crop.GetHeight());
        if(fix) accumulateX(deltaX, deltaY);
        crop = wxRect(crop.x, crop.y, crop.GetWidth() + deltaX, 
                crop.GetHeight() + deltaY);
    } else if(zonePressed() == ict::NW) {
        deltaX += - relativePress.x;
        deltaY += - relativePress.y;
        deltaX += target.x - crop.x;
        deltaY += target.y - crop.y;
        if(fix) accumulateX(deltaX, deltaY);
        crop = wxRect(crop.x + deltaX, crop.y + deltaY,
                crop.GetWidth() - deltaX, 
                crop.GetHeight() - deltaY);
    } else if(zonePressed() == ict::NE) {
        deltaX += - relativePress.x;
        deltaY += - relativePress.y;
        deltaX += target.x - (crop.x + crop.GetWidth());
        deltaY += target.y - crop.y;
        if(fix) { accumulateX(deltaX, deltaY); deltaX = -deltaX; }
        crop = wxRect(crop.x, crop.y + deltaY, 
                crop.GetWidth() + deltaX, 
                crop.GetHeight() - deltaY);
    } else if(zonePressed() == ict::SW) {
        deltaX += - relativePress.x;
        deltaY += - relativePress.y;
        deltaX += target.x - crop.x;
        deltaY += target.y - (crop.y + crop.GetHeight());
        if(fix) { accumulateX(deltaX, deltaY); deltaX = -deltaX; }
        crop = wxRect(crop.x + deltaX, crop.y, 
                crop.GetWidth() - deltaX, 
                crop.GetHeight() + deltaY);
    } else if(zonePressed() == ict::N) {
        deltaY += - relativePress.y;
        deltaY += target.y - crop.y;
        if(fix) accumulateX(deltaX, deltaY);
        crop = wxRect(crop.x + deltaX, crop.y + deltaY, 
                crop.GetWidth() - deltaX, 
                crop.GetHeight() - deltaY);
    } else if(zonePressed() == ict::S) {
        deltaY += - relativePress.y;
        deltaY += target.y - (crop.y + crop.GetHeight());
        if(fix) accumulateX(deltaX, deltaY);
        crop = wxRect(crop.x, crop.y, 
                crop.GetWidth() + deltaX, 
                crop.GetHeight() + deltaY);
    } else if(zonePressed() == ict::W) {
        deltaX += - relativePress.x;
        deltaX += target.x - crop.x;
        if(fix) { accumulateY(deltaY, deltaX); deltaY = -deltaY; }
        crop = wxRect(crop.x + deltaX, crop.y, 
                crop.GetWidth() - deltaX, 
                crop.GetHeight() + deltaY);
    } else if(zonePressed() == ict::E) {
        deltaX += - relativePress.x;
        deltaX += target.x - (crop.x + crop.GetWidth());
        if(fix) { accumulateY(deltaY, deltaX); deltaY = -deltaY; }
        crop = wxRect(crop.x, crop.y + deltaY, 
                crop.GetWidth() + deltaX, 
                crop.GetHeight() - deltaY);
    }
    fitInConstraint();
}

void CropController::pushToConstraint() {
    if(!activeCons) return;
    int cx1 = cons.GetX(), cx2 = cons.GetX() + cons.GetWidth();
    int nx1 = crop.GetX(), nx2 = crop.GetX() + crop.GetWidth();
    if(nx1 < cx1) crop.SetPosition(wxPoint(cx1, crop.GetY()));
    if(nx2 > cx2)
        crop.SetPosition(wxPoint(cx2 - crop.GetWidth(), crop.GetY()));
    int cy1 = cons.GetY(), cy2 = cons.GetY() + cons.GetHeight();
    int ny1 = crop.GetY(), ny2 = crop.GetY() + crop.GetHeight();
    if(ny1 < cy1) crop.SetPosition(wxPoint(crop.GetX(), cy1));
    if(ny2 > cy2)
        crop.SetPosition(wxPoint(crop.GetX(), cy2 - crop.GetHeight()));
}

void CropController::fitInConstraint() {
    if(!activeCons || cons.Contains(crop)) return;
    if(!fix) {
        crop.Intersect(cons);
        return;
    }
    bool exceedsTopY = crop.GetY() < cons.GetY();
    bool exceedsBottomY =
        (crop.GetY() + crop.GetHeight()) > (cons.GetY() + cons.GetHeight());
    bool exceedsLeftX = crop.GetX() < cons.GetX();
    bool exceedsRightX =
        (crop.GetX() + crop.GetWidth()) > (cons.GetX() + cons.GetWidth());
    wxRect aux(crop);
    aux.Intersect(cons);
    int newWidth, newHeight;
    if(fixHint == ict::SE || fixHint == ict::S) {
        if(exceedsRightX) {
            newWidth = aux.GetWidth();
            accumulateY(newHeight, newWidth);
            crop.SetSize(wxSize(newWidth, newHeight));
            exceedsBottomY =
                (crop.GetY() + crop.GetHeight()) >
                (cons.GetY() + cons.GetHeight());
        }
        if(exceedsBottomY) {
            newHeight = aux.GetHeight();
            accumulateX(newWidth, newHeight);
            crop.SetSize(wxSize(newWidth, newHeight));
        }
    } else if(fixHint == ict::SW || fixHint == ict::W) {
        if(exceedsLeftX) {
            newWidth = aux.GetWidth();
            accumulateY(newHeight, newWidth);
            crop.SetSize(wxSize(newWidth, newHeight));
            crop.SetPosition(wxPoint(cons.GetX(), crop.GetY()));
            exceedsBottomY =
                (crop.GetY() + crop.GetHeight()) >
                (cons.GetY() + cons.GetHeight());
        }
        if(exceedsBottomY) {
            newHeight = aux.GetHeight();
            accumulateX(newWidth, newHeight);
            wxPoint newPos(crop.GetX() + crop.GetWidth() - newWidth,
                           crop.GetY());
            crop.SetSize(wxSize(newWidth, newHeight));
            crop.SetPosition(newPos);
        }
    } else if(fixHint == ict::NE || fixHint == ict::E) {
        if(exceedsRightX) {
            newWidth = aux.GetWidth();
            accumulateY(newHeight, newWidth);
            wxPoint newPos(crop.GetX(),
                           crop.GetY() + crop.GetHeight() - newHeight);
            crop.SetSize(wxSize(newWidth, newHeight));
            crop.SetPosition(newPos);
            exceedsTopY = crop.GetY() < cons.GetY();
        }
        if(exceedsTopY) {
            newHeight = aux.GetHeight();
            accumulateX(newWidth, newHeight);
            crop.SetSize(wxSize(newWidth, newHeight));
            crop.SetPosition(wxPoint(crop.GetX(), cons.GetY()));
        }
    } else if(fixHint == ict::NW || fixHint == ict::N) {
        if(exceedsLeftX) {
            newWidth = aux.GetWidth();
            accumulateY(newHeight, newWidth);
            wxPoint newPos(cons.GetX(),
                           crop.GetY() + crop.GetHeight() - newHeight);
            crop.SetSize(wxSize(newWidth, newHeight));
            crop.SetPosition(newPos);
            exceedsTopY = crop.GetY() < cons.GetY();
        }
        if(exceedsTopY) {
            newHeight = aux.GetHeight();
            accumulateX(newWidth, newHeight);
            wxPoint newPos(crop.GetX() + crop.GetWidth() - newWidth,
                           cons.GetY());
            crop.SetSize(wxSize(newWidth, newHeight));
            crop.SetPosition(newPos);
        }
    }
}

void CropController::constraint(wxRect &cons) {
    this->cons = cons;
}

bool CropController::constraint(bool op) {
    activeCons = op;
    if(op) return cropRect(crop, true, accumX, accumY);
    else return false;
}

void CropController::accumulateX(int &dxToCalc, int &dyToUse) {
    accumX += (float)dyToUse * ratio;
    dxToCalc = std::floor(accumX);
    accumX -= dxToCalc;
}

void CropController::accumulateY(int &dyToCalc, int &dxToUse) {
    accumY += (float)dxToUse / ratio;
    dyToCalc = std::floor(accumY);
    accumY -= dyToCalc;
}

void CropController::fixRatio(bool op) {
    fix = op;
    if(fix) setRatio((float)crop.GetWidth() / (float)crop.GetHeight());
}

void CropController::setRatio(float r) {
    ratio = r;
}

bool CropController::cropSize(const wxSize &s) {
    if(s == crop.GetSize()) return false;
    return cropRect(wxRect(crop.GetPosition(),s));
}

bool CropController::
cropRect(const wxRect &r, bool holdRatio, float initAx, float initAy) {
    wxRect next(r);
    wxRect prevRect = crop;
    fixHint = ict::SE;
    if(!holdRatio) setRatio((float)r.GetWidth() / (float)r.GetHeight());
    accumX = initAx; accumY = initAy;
    if(activeCons && !cons.Contains(next.GetPosition())) {
        next.SetPosition(cons.GetPosition());
    }
    crop = next;
    fitInConstraint();
    if(prevRect != crop) {
        updateZones();
        return true;
    } else return false;
}

bool CropController::cropRect(const wxRect &r) {
    return cropRect(r, false, 0.0, 0.0);
}

