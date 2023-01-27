#include "controller.h"
#include "defs.h"
#include <iostream>

CropController::CropController() {

}

CropController::CropController(const wxRect &r) {
    crop = r;
    cons = r;
    pressZone = ict::NONE;
    updateSizes();
}

CropController::~CropController() {

}

void CropController::updateSizes() {
    int x1 = crop.GetX(), x2 = crop.GetX() + crop.GetWidth();
    int y1 = crop.GetY(), y2 = crop.GetY() + crop.GetHeight();
    iz = crop;
    nz = wxRect(x1, y1, crop.GetWidth(), ict::CORNER);
    sz = wxRect(x1, y2 - ict::CORNER, crop.GetWidth(), ict::CORNER);
    ez = wxRect(x2 - ict::CORNER, y1, ict::CORNER, crop.GetHeight());
    wz = wxRect(x1, y1, ict::CORNER, crop.GetHeight());
    nez = wxRect(x2 - ict::CORNER, y1, ict::CORNER, ict::CORNER);
    nwz = wxRect(x1, y1, ict::CORNER, ict::CORNER);
    sez = wxRect(x2 - ict::CORNER, y2 - ict::CORNER, ict::CORNER, ict::CORNER);
    swz = wxRect(x1, y2 - ict::CORNER, ict::CORNER, ict::CORNER);
}

ict::Zone CropController::getLocation(const wxPoint p) const {
    if(nez.Contains(p)) return ict::NE;
    if(nwz.Contains(p)) return ict::NW;
    if(sez.Contains(p)) return ict::SE;
    if(swz.Contains(p)) return ict::SW;
    if(nz.Contains(p)) return ict::N;
    if(sz.Contains(p)) return ict::S;
    if(ez.Contains(p)) return ict::E;
    if(wz.Contains(p)) return ict::W;
    if(iz.Contains(p)) return ict::INNER;
    return ict::NONE;
}

ict::Zone CropController::zonePressed() const {
    return pressZone;
}

void CropController::press(const wxPoint &p) {
    pressZone = getLocation(p);
    relativePress = relativeToZone(p, pressZone);
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
        updateSizes();
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
        deltaX += ict::CORNER - relativePress.x;
        deltaY += ict::CORNER - relativePress.y;
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
        deltaX += ict::CORNER - relativePress.x;
        deltaY += - relativePress.y;
        deltaX += target.x - (crop.x + crop.GetWidth());
        deltaY += target.y - crop.y;
        if(fix) { accumulateX(deltaX, deltaY); deltaX = -deltaX; }
        crop = wxRect(crop.x, crop.y + deltaY, 
                crop.GetWidth() + deltaX, 
                crop.GetHeight() - deltaY);
    } else if(zonePressed() == ict::SW) {
        deltaX += - relativePress.x;
        deltaY += ict::CORNER - relativePress.y;
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
        deltaY += ict::CORNER - relativePress.y;
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
        deltaX += ict::CORNER - relativePress.x;
        deltaX += target.x - (crop.x + crop.GetWidth());
        if(fix) { accumulateY(deltaY, deltaX); deltaY = -deltaY; }
        crop = wxRect(crop.x, crop.y + deltaY, 
                crop.GetWidth() + deltaX, 
                crop.GetHeight() - deltaY);
    }
    fitInConstraint();
}

wxRect CropController::rectZone(ict::Zone z) const {
    if(z == ict::NE) return nez;
    if(z == ict::NW) return nwz;
    if(z == ict::SE) return sez;
    if(z == ict::SW) return swz;
    if(z == ict::N) return nz;
    if(z == ict::S) return sz;
    if(z == ict::E) return ez;
    if(z == ict::W) return wz;
    if(z == ict::INNER) return iz;;
    return wxRect();
}

wxPoint CropController::relativeToZone(const wxPoint &p, ict::Zone z) {
    if(!z) return p;
    wxRect rz(rectZone(z));
    return wxPoint(p - rz.GetPosition());
}

wxRect CropController::relativeToCrop(ict::Zone z) const {
    wxRect rel(rectZone(z));
    rel.SetPosition(rel.GetPosition() - crop.GetPosition());
    return rel;
}

void CropController::pushToConstraint() {
    if(!activeCons) return;
    int cx1 = cons.GetX(), cx2 = cons.GetX() + cons.GetWidth();
    int nx1 = crop.GetX(), nx2 = crop.GetX() + crop.GetWidth();
    if(nx1 < cx1) crop.SetPosition(wxPoint(cx1, crop.GetY()));
    if(nx2 > cx2) crop.SetPosition(wxPoint(cx2 - crop.GetWidth(), crop.GetY()));
    int cy1 = cons.GetY(), cy2 = cons.GetY() + cons.GetHeight();
    int ny1 = crop.GetY(), ny2 = crop.GetY() + crop.GetHeight();
    if(ny1 < cy1) crop.SetPosition(wxPoint(crop.GetX(), cy1));
    if(ny2 > cy2) crop.SetPosition(wxPoint(crop.GetX(), cy2 - crop.GetHeight()));
}

void CropController::fitInConstraint() {
    if(!activeCons || cons.Contains(crop)) return;
    if(!fix) {
        crop.Intersect(cons);
        return;
    }
    bool exceedsTopY = crop.GetY() < cons.GetY();
    bool exceedsBottomY = (crop.GetY() + crop.GetHeight()) > (cons.GetY() + cons.GetHeight());
    bool exceedsLeftX = crop.GetX() < cons.GetX();
    bool exceedsRightX = (crop.GetX() + crop.GetWidth()) > (cons.GetX() + cons.GetWidth());
    wxRect aux(crop);
    aux.Intersect(cons);
    int newWidth, newHeight;
    if(fixHint == ict::SE || fixHint == ict::S) {
        if(exceedsRightX) {
            newWidth = aux.GetWidth();
            accumulateY(newHeight, newWidth);
            crop.SetSize(wxSize(newWidth, newHeight));
            exceedsBottomY = (crop.GetY() + crop.GetHeight()) > (cons.GetY() + cons.GetHeight());
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
            exceedsBottomY = (crop.GetY() + crop.GetHeight()) > (cons.GetY() + cons.GetHeight());
        }
        if(exceedsBottomY) {
            newHeight = aux.GetHeight();
            accumulateX(newWidth, newHeight);
            wxPoint newPos(crop.GetX() + crop.GetWidth() - newWidth, crop.GetY());
            crop.SetSize(wxSize(newWidth, newHeight));
            crop.SetPosition(newPos);
        }
    } else if(fixHint == ict::NE || fixHint == ict::E) {
        if(exceedsRightX) {
            newWidth = aux.GetWidth();
            accumulateY(newHeight, newWidth);
            wxPoint newPos(crop.GetX(), crop.GetY() + crop.GetHeight() - newHeight);
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
            wxPoint newPos(cons.GetX(), crop.GetY() + crop.GetHeight() - newHeight);
            crop.SetSize(wxSize(newWidth, newHeight));
            crop.SetPosition(newPos);
            exceedsTopY = crop.GetY() < cons.GetY();
        }
        if(exceedsTopY) {
            newHeight = aux.GetHeight();
            accumulateX(newWidth, newHeight);
            wxPoint newPos(crop.GetX() + crop.GetWidth() - newWidth, cons.GetY());
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

bool CropController::cropRect(const wxRect &r, bool holdRatio, float initAx, float initAy) {
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
        updateSizes();
        return true;
    } else return false;
}

bool CropController::cropRect(const wxRect &r) {
    return cropRect(r, false, 0.0, 0.0);
}

