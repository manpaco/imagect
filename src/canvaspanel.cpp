#include "canvaspanel.h"
#include "cropevent.h"
#include "defs.h"
#include <wx/graphics.h>
#include "wx/dcbuffer.h"
#include <Magick++.h>
#include "imgtools.h"

CanvasPanel::CanvasPanel(wxWindow *parent, wxWindowID id) {
    Create(parent, id);
}

CanvasPanel::CanvasPanel(wxWindow *parent, wxWindowID id, Magick::Image *img):
    CanvasPanel(parent, id) {
    if(!GetParent()) return;
    wxSize imgSz(img->columns(), img->rows());
    wxSize aux(((imgSz * ict::IMG_MULTIPLIER) - imgSz) / 2);
    wxPoint imgPos(aux.GetWidth(), aux.GetHeight());
    imgRect = wxRect(imgPos, imgSz);
    controller = CropController(imgRect);
    prevCrop = imgRect;
    compressImage(img);
    wxBitmap initBm(createImage(*img));
    initCanvas(initBm);
    Bind(wxEVT_MOTION, &CanvasPanel::mouseMotion, this);
    Bind(wxEVT_LEFT_DOWN, &CanvasPanel::mousePress, this);
    Bind(wxEVT_LEFT_UP, &CanvasPanel::mouseRelease, this);
    Bind(wxEVT_PAINT, &CanvasPanel::onPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent &){});
}

void CanvasPanel::mouseMotion(wxMouseEvent &event) {
    ict::Zone lastZone(controller.
            getLocation(translatePoint(lastPoint, ict::SNC_T, ict::OUT_D)));
    wxPoint currentPoint(event.GetPosition());
    ict::Zone currentZone(controller.
            getLocation(translatePoint(currentPoint, ict::SNC_T, ict::OUT_D)));
    if(!controller.zonePressed()) {
        if(lastZone != currentZone) changeCursor(currentZone);
    } else {
        if(controller.
                modify(translatePoint(currentPoint, ict::SNC_T, ict::OUT_D))) {
            refreshDamaged();
            sendCropEvent();
        }
    }
    lastPoint = currentPoint;
    event.Skip();
}

void CanvasPanel::mousePress(wxMouseEvent &event) {
    if(!HasCapture()) CaptureMouse();
    controller.
        press(translatePoint(event.GetPosition(), ict::SNC_T, ict::OUT_D));
    event.Skip();
}

void CanvasPanel::mouseRelease(wxMouseEvent &event) {
    if(HasCapture()) ReleaseMouse();
    controller.release();
    changeCursor(controller.
        getLocation(translatePoint(lastPoint, ict::SNC_T, ict::OUT_D)));
    event.Skip();
}

void CanvasPanel::sendCropEvent() {   
    wxSize cs(cropSize());
    wxPoint co(cropOffset());
    CropEvent toSend(EVT_CROP_CHANGE, GetId(), cs, co);
    toSend.SetEventObject(this);
    ProcessWindowEvent(toSend);
}

void CanvasPanel::refreshDamaged() {
    wxRect damaged(translateRect(prevCrop, ict::SNC_T, ict::IN_D).
        Union(translateRect(controller.cropRect(), ict::SNC_T, ict::IN_D)));
    wxWindow *myParent(GetParent());
    if(myParent) {
        wxRect viewRect(myParent->GetScreenRect());
        viewRect.SetPosition(ScreenToClient(viewRect.GetPosition()));
        damaged.Intersect(viewRect);
    }
    damaged.Inflate(ict::CORNER, ict::CORNER);
    RefreshRect(damaged);
    prevCrop = controller.cropRect();
}

void CanvasPanel::onPaint(wxPaintEvent &event) {
    wxRect damaged(GetUpdateRegion().GetBox());
    damaged = translateRect(damaged, ict::SCALE_T, ict::OUT_D);
    wxBufferedPaintDC canvasPainter(this);
    canvasPainter.SetUserScale(scaleFactor, scaleFactor);
    canvasPainter.
        DrawBitmap(buffer->GetSubBitmap(damaged), damaged.GetPosition());
    canvasPainter.SetUserScale(1, 1);
    wxGraphicsContext *gcd = wxGraphicsContext::Create(canvasPainter);
    if(gcd) {
        paintCropRect(translateRect(controller.cropRect(),
                                    ict::SNC_T, ict::IN_D),
                      gcd);
        delete gcd;
    }
    event.Skip();
}

void CanvasPanel::paintCropRect(const wxRect &paint, wxGraphicsContext *gc) {
    wxPen wLine(wxColour(*wxWHITE), 1);
    wxPen bLine(wxColour(*wxBLACK), 1);
    gc->SetBrush(wxBrush(wxColour(0, 0, 0, 0)));
    gc->SetPen(bLine);
    gc->DrawRectangle(paint.GetX(),
                      paint.GetY(),
                      paint.GetWidth() - 1,
                      paint.GetHeight() - 1);
    gc->SetPen(wLine);
    gc->DrawRectangle(paint.GetX() + 1,
                      paint.GetY() + 1,
                      paint.GetWidth() - 3,
                      paint.GetHeight() - 3);
    gc->SetPen(bLine);
    gc->DrawRectangle(paint.GetX() + 2,
                      paint.GetY() + 2,
                      paint.GetWidth() - 5,
                      paint.GetHeight() - 5);
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
    if(g->GetSize().x < 0) g->SetWidth(imgRect.GetWidth());
    if(g->GetSize().y < 0) g->SetHeight(imgRect.GetHeight());
    wxPoint newPos(absoluteCoords(g->GetPosition()));
    wxSize newSz(g->GetSize());
    wxRect newR(newPos, newSz);
    if(controller.cropRect(newR)) refreshDamaged();
    if(newR != controller.cropRect()) {
        *g = controller.cropRect();
        g->SetPosition(relativeToImage(g->GetPosition()));
        return false;
    } else return true;
}

wxPoint CanvasPanel::relativeToImage(const wxPoint &ap) const {
    return wxPoint(ap - imgRect.GetPosition());
}

wxPoint CanvasPanel::absoluteCoords(const wxPoint &rp) const {
    return wxPoint(rp + imgRect.GetPosition());
}

void CanvasPanel::fixCrop(bool op) {
    controller.fixRatio(op);
}

bool CanvasPanel::allowGrow(bool op) {
    if(controller.constraint(!op)) {
        refreshDamaged();
        return true;
    } else return false;
}

wxPoint CanvasPanel::cropOffset() const {
    return relativeToImage(controller.cropPosition());
}

bool CanvasPanel::cropSize(wxSize *s) {
    if(controller.cropSize(*s)) refreshDamaged();
    if(controller.cropSize() != *s) {
        *s = controller.cropSize();
        return false;
    } else return true;
}

wxSize CanvasPanel::cropSize() const {
    return controller.cropSize();
}

wxRect CanvasPanel::cropGeometry() const {
    return wxRect(cropOffset(), cropSize());
}

int CanvasPanel::translate(int v, ict::Tot t, ict::Dot d) const {
    if(t == ict::SCALE_T) {
        if(d == ict::IN_D) return v * scaleFactor;
        if(d == ict::OUT_D) return v / scaleFactor;
    } else if(t == ict::COMPRESS_T) {
        if(d == ict::IN_D) return v * compressFactor;
        if(d == ict::OUT_D) return v / compressFactor;
    } else if(t == ict::SNC_T) {
        if(d == ict::IN_D) return v * scaleFactor * compressFactor;
        if(d == ict::OUT_D) return v / scaleFactor / compressFactor;
    }
    return 0;
}

wxRect CanvasPanel::
translateRect(const wxRect &r, ict::Tot t, ict::Dot d) const {
    return wxRect(translatePoint(r.GetPosition(), t, d),
                  translateSize(r.GetSize(), t, d));
}

wxPoint CanvasPanel::
translatePoint(const wxPoint &p, ict::Tot t, ict::Dot d) const {
    return wxPoint(translate(p.x, t, d), translate(p.y, t, d));
}

wxSize CanvasPanel::
translateSize(const wxSize &s, ict::Tot t, ict::Dot d) const {
    wxPoint aux(s.GetWidth(), s.GetHeight());
    aux = translatePoint(aux, t, d);
    return wxSize(aux.x, aux.y);
}

void CanvasPanel::refreshCanvas() {
    updateSizes();
    Refresh();
}

void CanvasPanel::setScaleFactor(float sf) {
    if(sf == scaleFactor) return;
    scaleFactor = sf;
    refreshCanvas();
}

void CanvasPanel::initCanvas(wxBitmap &bm) {
    if(!bm.IsOk()) return;
    initBuffer(bm);
    refreshCanvas();
}

void CanvasPanel::updateSizes() {
    wxSize newSize(translateSize(bufferSize, ict::SCALE_T, ict::IN_D));
    SetMinSize(newSize);
    SetSize(newSize);
}

wxRect CanvasPanel::shadowRect() {
    wxPoint posOff(ict::SHADOW_OFFSET, ict::SHADOW_OFFSET);
    wxPoint shPos(translatePoint
            (imgRect.GetPosition(), ict::COMPRESS_T, ict::IN_D) - posOff);
    wxSize szOff(ict::SHADOW_OFFSET * 2, ict::SHADOW_OFFSET * 2);
    wxSize shSz(translateSize
            (imgRect.GetSize(), ict::COMPRESS_T, ict::IN_D) + szOff);
    return wxRect(shPos, shSz);
}

void CanvasPanel::initBuffer(wxBitmap &bm) {
    if(buffer) delete buffer;
    buffer = new wxBitmap(bm.GetSize() * ict::IMG_MULTIPLIER);
    bufferSize = buffer->GetSize();
    wxMemoryDC memBb(*buffer);
    wxBrush backBrush(wxColour(188, 188, 188));
    memBb.SetBrush(backBrush);
    memBb.DrawRectangle(0, 0, buffer->GetWidth(), buffer->GetHeight());
    wxBrush shadowBrush(wxColour(91, 91, 91));
    memBb.SetBrush(shadowBrush);
    memBb.DrawRectangle(shadowRect());
    memBb.DrawBitmap(bm,
            translatePoint(imgRect.GetPosition(), ict::COMPRESS_T, ict::IN_D));
}

void CanvasPanel::compressImage(Magick::Image *img) {
    wxWindow *myParent = GetParent();
    Magick::Geometry geoImg(img->columns(), img->rows());
    Magick::Geometry 
        geoMax(myParent->GetSize().GetWidth() / ict::IMG_MULTIPLIER,
               myParent->GetSize().GetHeight() / ict::IMG_MULTIPLIER);
    if(geoImg.width() <= geoMax.width() && geoImg.height() <= geoMax.height())
        return;
    compressFactor = factorToFit(geoMax, geoImg);
    geoImg.width(geoImg.width() * compressFactor);
    geoImg.height(geoImg.height() * compressFactor);
    img->zoom(geoImg);
}

CanvasPanel::~CanvasPanel() {
    if(buffer) delete buffer;
}
