#include "canvaspanel.h"
#include "defs.h"
#include "rectangle.h"
#include "cropevent.h"
#include <wx/graphics.h>

CanvasPanel::CanvasPanel(wxWindow *parent, wxWindowID id, const wxBitmap &bm) :
        wxScrolledCanvas() {

    Create(parent, id);
    createObjects(bm);
    initObjects();
    Bind(wxEVT_PAINT, &CanvasPanel::onPaint, this);
    Bind(wxEVT_SIZE, &CanvasPanel::updatePositions, this);
    Bind(wxEVT_SCROLLWIN_THUMBTRACK, &CanvasPanel::updateScrollValues, this);
    Bind(EVT_RECTANGLE_CHANGE, &CanvasPanel::reportCropChange, this);
}

wxPoint CanvasPanel::relativeCoords(const wxPoint &clientPoint) const {
    wxPoint relative(clientPoint);
    relative.x -= imgPosition.x;
    relative.y -= imgPosition.y;
    return relative;
}

wxPoint CanvasPanel::absoluteCoords(const wxPoint &clientPoint) const {
    wxPoint absolute(clientPoint);
    if(!glitchX) absolute.x += scrollPosition.x * ppuX;
    if(!glitchY) absolute.y += scrollPosition.y * ppuY;
    return absolute;
}

void CanvasPanel::reportCropChange(wxCommandEvent &event) {
    wxSize size(cropArea->GetSize());
    wxPoint offset = relativeCoords(cropArea->GetPosition());
    CropEvent toSend(EVT_CROP_CHANGE, GetId(), size, offset);
    toSend.SetEventObject(this);
    ProcessWindowEvent(toSend);
}

void CanvasPanel::updateScrollValues(wxScrollWinEvent &event) {
    if(event.GetOrientation() == wxHORIZONTAL) {
        glitchX = GetSize().GetWidth() > virtualSize.GetWidth();
        scrollPosition.x = event.GetPosition();
    } else {
        glitchY = GetSize().GetHeight() > virtualSize.GetHeight();
        scrollPosition.y = event.GetPosition();
    }
    event.Skip();
}

void CanvasPanel::updatePositions(wxSizeEvent &event) {
    wxSize panelSize = event.GetSize();
    if(oldSize == panelSize) {
        event.Skip();
        return;
    }
    bool defaultX = false, defaultY = false;
    defaultX = (oldSize.GetWidth() <= virtualSize.GetWidth()) && (panelSize.GetWidth() <= virtualSize.GetWidth());
    defaultY = (oldSize.GetHeight() <= virtualSize.GetHeight()) && (panelSize.GetHeight() <= virtualSize.GetHeight());

    bool crossVirtualX = false, crossVirtualY = false;
    crossVirtualX = (oldSize.GetWidth() <= virtualSize.GetWidth() && panelSize.GetWidth() > virtualSize.GetWidth()) ||
                    (oldSize.GetWidth() > virtualSize.GetWidth() && panelSize.GetWidth() <= virtualSize.GetWidth());
    crossVirtualY = (oldSize.GetHeight() <= virtualSize.GetHeight() && panelSize.GetHeight() > virtualSize.GetHeight()) ||
                    (oldSize.GetHeight() > virtualSize.GetHeight() && panelSize.GetHeight() <= virtualSize.GetHeight());

    int deltaX = 0, deltaY = 0;
    bool toRight = false, toBottom = false;
    toRight = (panelSize.GetWidth() - oldSize.GetWidth()) > 0;
    toBottom = (panelSize.GetHeight() - oldSize.GetHeight()) > 0;
    if(crossVirtualX && !defaultX) {
        if(toRight) deltaX = panelSize.GetWidth() - virtualSize.GetWidth();
        else deltaX = virtualSize.GetWidth() - oldSize.GetWidth();
    }
    if(!crossVirtualX && !defaultX) {
        deltaX = panelSize.GetWidth() - oldSize.GetWidth();
    }
    if(crossVirtualY && !defaultY) {
        if(toBottom) deltaY = panelSize.GetHeight() - virtualSize.GetHeight();
        else deltaY = virtualSize.GetHeight() - oldSize.GetHeight();
    }
    if(!crossVirtualY && !defaultY) {
        deltaY = panelSize.GetHeight() - oldSize.GetHeight();
    }

    wxPoint target(cropArea->GetPosition());
    GetScrollPixelsPerUnit(&ppuX, &ppuY);
    if(crossVirtualX && toRight) {
        target.x += scrollPosition.x * ppuX;
        scrollPosition.x = 0;
    }
    if(crossVirtualY && toBottom) {
        target.y += scrollPosition.y * ppuY;
        scrollPosition.y = 0;
    }
    if(glitchX) target.x += scrollPosition.x * ppuX;
    if(glitchY) target.y += scrollPosition.y * ppuY;
    if(!defaultX) scrollPosition.x = 0;
    if(!defaultY) scrollPosition.y = 0;
    target.x += deltaX / 2;
    target.y += deltaY / 2;
    cropArea->Move(target);

    imgPosition.x += deltaX / 2;
    imgPosition.y += deltaY / 2;

    oldSize = panelSize;

    Update();
    event.Skip();
}

void CanvasPanel::createObjects(const wxBitmap &bm) {
    if(bm.IsOk()) {
        img = new wxBitmap(bm);
        cropArea = new Rectangle(this, ict::CROP_AREA);
    } else {
        img = nullptr;
        cropArea = nullptr;
    }
}

void CanvasPanel::initObjects() {
    if(img) {
        SetMinSize(wxSize(200, 200));
        ppuX = 5; ppuY = 5;
        SetScrollRate(ppuX, ppuY);
        virtualSize.SetWidth((virtualOffset * 2) + img->GetWidth()); 
        virtualSize.SetHeight((virtualOffset * 2) + img->GetHeight());
        SetVirtualSize(virtualSize);
        imgPosition.x = virtualOffset;
        imgPosition.y = virtualOffset;
        oldSize = GetSize();
        cropArea->SetSize(imgPosition.x, imgPosition.y, img->GetWidth(), img->GetHeight());
    }
}

void CanvasPanel::onPaint(wxPaintEvent &event) {
    wxPaintDC dev(this);
    DoPrepareDC(dev);
    wxGraphicsContext *gcd = wxGraphicsContext::Create(dev);
    if(img && gcd) {
        dev.DrawBitmap(*img, wxPoint(imgPosition.x, imgPosition.y));
        wxColour blk(0, 0, 0, 100);
        gcd->SetBrush(wxBrush(blk));
        gcd->DrawRectangle(0, 0, 200, 200);
    }
}
