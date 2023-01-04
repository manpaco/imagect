#include "canvaspanel.h"
#include "cropevent.h"
#include "defs.h"
#include <wx/graphics.h>

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

CanvasPanel::CanvasPanel(wxWindow *parent, wxWindowID id) {
    Create(parent, id);
}

CanvasPanel::CanvasPanel(wxWindow *parent, wxWindowID id, wxBitmap &bm): CanvasPanel(parent, id) {
    initCanvas(bm);
    Bind(wxEVT_MOTION, &CanvasPanel::mouseMotion, this);
    Bind(wxEVT_LEFT_DOWN, &CanvasPanel::mousePress, this);
    Bind(wxEVT_LEFT_UP, &CanvasPanel::mouseRelease, this);
    Bind(wxEVT_PAINT, &CanvasPanel::onPaint, this);
}

void CanvasPanel::mouseMotion(wxMouseEvent &event) {
    ict::Zone lastZone(controller.getLocation(translatePointOut(lastPoint)));
    wxPoint currentPoint(event.GetPosition());
    ict::Zone currentZone(controller.getLocation(translatePointOut(currentPoint)));
    if(!controller.zonePressed()) {
        if(lastZone != currentZone) changeCursor(currentZone);
    } else {
        if(controller.modify(translatePointOut(currentPoint))) sendCropEvent();
    }
    lastPoint = currentPoint;
    event.Skip();
}

void CanvasPanel::mousePress(wxMouseEvent &event) {
    if(!HasCapture()) CaptureMouse();
    controller.press(translatePointOut(event.GetPosition()));
    event.Skip();
}

void CanvasPanel::mouseRelease(wxMouseEvent &event) {
    if(HasCapture()) ReleaseMouse();
    controller.release();
    changeCursor(controller.getLocation(translatePointOut(lastPoint)));
    event.Skip();
}

void CanvasPanel::sendCropEvent() {   
    wxSize cs(controller.cropSize());
    wxPoint co(relativeToImage(controller.cropPosition()));
    CropEvent toSend(EVT_CROP_CHANGE, GetId(), cs, co);
    toSend.SetEventObject(this);
    ProcessWindowEvent(toSend);
    updatePaintBuffer();
    prevCrop = controller.cropRect();
}

void CanvasPanel::onPaint(wxPaintEvent &event) {
    wxPaintDC painter(this);
    wxRegionIterator it(GetUpdateRegion());
    while(it) {
        wxRect upd(it.GetX(), it.GetY(), it.GetW(), it.GetH());
        painter.DrawBitmap(paintBuffer->GetSubBitmap(upd), upd.GetPosition());
        it++;
    }
}

void CanvasPanel::updatePaintBuffer(bool force) {
    if(prevCrop == controller.cropRect() && !force) return;
    wxMemoryDC painter(*paintBuffer);
    wxRect tpc(translateRectIn(prevCrop));
    if(prevCrop != controller.cropRect()) painter.DrawBitmap(baseBuffer->GetSubBitmap(tpc), tpc.GetPosition());
    wxRect tcr(translateRectIn(controller.cropRect()));
    painter.DrawBitmap(createCropBitmap(), tcr.GetPosition());
    wxRect damaged(tpc);
    if(force) damaged = wxRect(0, 0, paintBuffer->GetWidth(), paintBuffer->GetHeight());
    else damaged.Union(tcr);
    RefreshRect(damaged);
}

void CanvasPanel::paintSpecialFrame(const wxRect &paint, wxGraphicsContext *gc, bool fill) {
    wxPen wLine(wxColour(*wxWHITE), 1);
    wxPen bLine(wxColour(*wxBLACK), 1);
    gc->SetBrush(wxBrush(wxColour(0, 0, 0, 0)));
    gc->SetPen(bLine);
    gc->DrawRectangle(paint.GetX(), paint.GetY(), paint.GetWidth() - 1, paint.GetHeight() - 1);
    gc->SetPen(wLine);
    gc->DrawRectangle(paint.GetX() + 1, paint.GetY() + 1, paint.GetWidth() - 3, paint.GetHeight() - 3);
    gc->SetPen(bLine);
    if(fill) { 
        gc->SetBrush(wxBrush(wxColour(*wxWHITE)));
        gc->SetPen(wxPen(wxColour(*wxWHITE)));
    }
    gc->DrawRectangle(paint.GetX() + 2, paint.GetY() + 2, paint.GetWidth() - 5, paint.GetHeight() - 5);
}

wxBitmap CanvasPanel::createCropBitmap() {
    wxRect tcr(translateRectIn(controller.cropRect()));
    wxBitmap cb(baseBuffer->GetSubBitmap(tcr));
    wxMemoryDC memCb(cb);
    wxGraphicsContext *gcd = wxGraphicsContext::Create(memCb);
    if(gcd) {
        paintSpecialFrame(wxRect(0, 0, tcr.GetWidth(), tcr.GetHeight()), gcd, false);
        paintSpecialFrame(translateRectIn(controller.relativeToCrop(ict::NW)), gcd, true);
        paintSpecialFrame(translateRectIn(controller.relativeToCrop(ict::NE)), gcd, true);
        paintSpecialFrame(translateRectIn(controller.relativeToCrop(ict::SE)), gcd, true);
        paintSpecialFrame(translateRectIn(controller.relativeToCrop(ict::SW)), gcd, true);
        delete gcd;
    }
    return cb;
}

void CanvasPanel::changeCursor(ict::Zone type) {
    if(type == ict::NE || type == ict::SW) 
        wxSetCursor(wxCursor(wxCURSOR_SIZENESW));
    if(type == ict::NW || type == ict::SE) 
        wxSetCursor(wxCursor(wxCURSOR_SIZENWSE));
    if(type == ict::N || type == ict::S) 
        wxSetCursor(wxCursor(wxCURSOR_SIZENS));
    if(type == ict::E || type == ict::W) 
        wxSetCursor(wxCursor(wxCURSOR_SIZEWE));
    if(type == ict::INNER)
        wxSetCursor(wxCursor(wxCURSOR_HAND));
    if(type == ict::NONE)
        wxSetCursor(wxNullCursor);
}

bool CanvasPanel::cropGeometry(wxRect *g) {
    wxPoint newPos(absoluteCoords(g->GetPosition()));
    wxSize newSz(g->GetSize());
    if(g->GetSize().x < 0) newSz.SetWidth(srcImgRect.GetWidth());
    if(g->GetSize().y < 0) newSz.SetHeight(srcImgRect.GetHeight());
    wxRect newG(newPos, newSz);
    if(controller.cropRect(newG)) sendCropEvent();
    newG = controller.cropRect();
    newG.SetPosition(relativeToImage(newG.GetPosition()));
    if(newG != *g) {
        *g = newG;
        return true;
    } else return false;    
}

wxPoint CanvasPanel::relativeToImage(const wxPoint &ap, bool scaled) const {
    wxPoint rel;
    if(scaled) rel = wxPoint(ap - translatePointIn(srcImgRect.GetPosition()));
    else rel = wxPoint(ap - srcImgRect.GetPosition());
    return rel;
}

wxPoint CanvasPanel::absoluteCoords(const wxPoint &rp, bool scaled) const {
    wxPoint abs;
    if(scaled) abs = wxPoint(rp + translatePointIn(srcImgRect.GetPosition()));
    else abs = wxPoint(rp + srcImgRect.GetPosition());
    return abs;
}

void CanvasPanel::fixCrop(bool op) {
    controller.fixRatio(op);
}

void CanvasPanel::allowGrow(bool op) {
    if(controller.constraint(!op)) sendCropEvent();
}

wxPoint CanvasPanel::cropOffset() const {
    return relativeToImage(controller.cropPosition());
}

bool CanvasPanel::cropSize(wxSize *s) {
    if(controller.cropSize(*s)) sendCropEvent();
    if(controller.cropSize() != *s) {
        *s = controller.cropSize();
        return true;
    }
    return false;
}

wxSize CanvasPanel::cropSize() const {
    return controller.cropSize();
}

int CanvasPanel::translateIn(int v) const {
    return v * scaleFactor;
}

int CanvasPanel::translateOut(int v) const {
    return v / scaleFactor;
}

wxRect CanvasPanel::translateRectIn(const wxRect &r) const {
    return wxRect(translateIn(r.GetX()), translateIn(r.GetY()), 
            translateIn(r.GetWidth()), translateIn(r.GetHeight()));
}

wxRect CanvasPanel::translateRectOut(const wxRect &r) const {
    return wxRect(translateOut(r.GetX()), translateOut(r.GetY()), 
            translateOut(r.GetWidth()), translateOut(r.GetHeight()));
}

wxPoint CanvasPanel::translatePointIn(const wxPoint &p) const {
    return wxPoint(translateIn(p.x), translateIn(p.y));
}

wxPoint CanvasPanel::translatePointOut(const wxPoint &p) const {
    return wxPoint(translateOut(p.x), translateOut(p.y));
}

wxSize CanvasPanel::translateSizeIn(const wxSize &s) const {
    return wxSize(translateIn(s.GetWidth()), translateIn(s.GetHeight()));
}

wxSize CanvasPanel::translateSizeOut(const wxSize &s) const {
    return wxSize(translateOut(s.GetWidth()), translateOut(s.GetHeight()));
}

void CanvasPanel::refreshCanvas() {
    initSizes();
    initBuffers();
}

void CanvasPanel::setScaleFactor(float sf) {
    if(sf == scaleFactor) return;
    scaleFactor = sf;
    refreshCanvas();
}

void CanvasPanel::initCanvas(wxBitmap &bm) {
    if(!bm.IsOk()) return;
    img = new wxBitmap(bm);
    srcImgRect = wxRect(bm.GetWidth(), bm.GetHeight(), bm.GetWidth(), bm.GetHeight());
    controller = CropController(srcImgRect);
    prevCrop = srcImgRect;
    refreshCanvas();
}

void CanvasPanel::initSizes() {
    wxSize newSize(srcImgRect.GetSize() * ict::IMG_MULTIPLIER);
    SetMinSize(translateSizeIn(newSize));
    SetSize(GetMinSize());
    img->SetWidth(translateIn(srcImgRect.GetWidth()));
    img->SetHeight(translateIn(srcImgRect.GetHeight()));
}

wxRect CanvasPanel::shadowRect() {
    wxPoint posOff(ict::SHADOW_OFFSET, ict::SHADOW_OFFSET);
    wxPoint shPos(translatePointIn(srcImgRect.GetPosition()) - posOff);
    wxSize szOff(ict::SHADOW_OFFSET * 2, ict::SHADOW_OFFSET * 2);
    wxSize shSz(translateSizeIn(srcImgRect.GetSize()) + szOff);
    return wxRect(shPos, shSz);
}

void CanvasPanel::initBuffers() {
    if(paintBuffer) delete paintBuffer;
    if(baseBuffer) delete baseBuffer;
    baseBuffer = new wxBitmap(GetSize());
    wxMemoryDC memBb(*baseBuffer);
    wxBrush backBrush(wxColour(188, 188, 188));
    memBb.SetBrush(backBrush);
    memBb.DrawRectangle(0, 0, baseBuffer->GetWidth(), baseBuffer->GetHeight());
    wxBrush shadowBrush(wxColour(91, 91, 91));
    memBb.SetBrush(shadowBrush);
    memBb.DrawRectangle(shadowRect());
    memBb.DrawBitmap(*img, translatePointIn(srcImgRect.GetPosition()));
    paintBuffer = new wxBitmap(*baseBuffer);
    updatePaintBuffer(true);
}

CanvasPanel::~CanvasPanel() {
    if(paintBuffer) delete paintBuffer;
    if(baseBuffer) delete baseBuffer;
    if(img) delete img;
}
