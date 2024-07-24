#include <cmath>
#include <math.h>
#include <iostream>
#include "extendedcanvas.hpp"
#include "canvasitem.hpp"
#include "defs.hpp"
#include "scaler.hpp"
#include <wx/dcmemory.h>
#include <wx/dcclient.h>
#include <wx/defs.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/scrolbar.h>
#include <wx/utils.h>
#include "zoomctrl.hpp"
#include "zoomevent.hpp"

ExtendedCanvas::ExtendedCanvas(wxWindow *parent, wxWindowID id) : wxWindow(parent, id) {
    layout = new wxFlexGridSizer(3, 2, 0, 0);
    canvas = new wxWindow(this, wxID_ANY);
    canvas->SetBackgroundStyle(wxBG_STYLE_PAINT);
    canvasReference = wxPoint2DDouble(0, 0);
    vBar = new wxScrollBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);
    hBar = new wxScrollBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
    zoom = new ZoomCtrl(this, ict::ZOOM_CT);
    generic = new wxWindow(this, wxID_ANY);
    scaler = new Scaler(1.0, 1.0);
    canvasBuffer = nullptr;
    pressedItem = nullptr;
    selectedItem = nullptr;
    grid = false;
    ctrlPressed = false;
    shiftPressed = false;
    layout->AddGrowableCol(0);
    layout->AddGrowableRow(0);
    layout->Add(canvas, 1, wxEXPAND);
    initScrollbars();
    layout->Add(vBar, 1, wxEXPAND);
    layout->Add(hBar, 1, wxEXPAND);
    layout->Add(generic, 1, wxEXPAND);
    wxBoxSizer *vertToolBarSizer = new wxBoxSizer(wxVERTICAL);
    vertToolBarSizer->AddSpacer(ict::BEST_SPACE);
    wxBoxSizer *toolBarSizer = new wxBoxSizer(wxHORIZONTAL);
    toolBarSizer->Add(zoom);
    vertToolBarSizer->Add(toolBarSizer);
    vertToolBarSizer->AddSpacer(ict::BEST_SPACE);
    layout->Add(vertToolBarSizer, 0, wxALIGN_CENTRE_HORIZONTAL);
    SetSizer(layout);
    canvas->Bind(wxEVT_PAINT, &ExtendedCanvas::paintCanvas, this);
    canvas->Bind(wxEVT_MOTION, &ExtendedCanvas::mouseMotion, this);
    canvas->Bind(wxEVT_LEFT_DOWN, &ExtendedCanvas::mousePress, this);
    canvas->Bind(wxEVT_LEFT_UP, &ExtendedCanvas::mouseRelease, this);
    canvas->Bind(wxEVT_SIZE, &ExtendedCanvas::resizeCanvas, this);
    canvas->Bind(wxEVT_MOUSEWHEEL, &ExtendedCanvas::mouseWheel, this);
    canvas->Bind(wxEVT_KEY_DOWN, &ExtendedCanvas::keyDown, this);
    canvas->Bind(wxEVT_KEY_UP, &ExtendedCanvas::keyUp, this);
    zoom->Bind(EVT_ZOOM_CHANGE, &ExtendedCanvas::onZoomChange, this, ict::ZOOM_CT);
    hBar->Bind(wxEVT_SCROLL_CHANGED, &ExtendedCanvas::horizontalScroll, this);
    vBar->Bind(wxEVT_SCROLL_CHANGED, &ExtendedCanvas::verticalScroll, this);
    generic->Bind(wxEVT_LEFT_DOWN, &ExtendedCanvas::gridToggle, this);
}

void ExtendedCanvas::onZoomChange(ZoomEvent &event) {
    scaler->setNewFactor(event.getScaleFactor(), event.getScaleFactor());
    wxPoint centre;
    centre.x = canvas->GetSize().x / 2;
    centre.y = canvas->GetSize().y / 2;
    doMagnify(centre);
}

void ExtendedCanvas::initScrollbars() {
    hBar->SetScrollbar(0, 50, 50, 50);
    vBar->SetScrollbar(0, 50, 50, 50);
    prevPosBars = wxPoint(0, 0);
}

void ExtendedCanvas::horizontalScroll(wxScrollEvent &event) {
    doScroll(wxPoint(event.GetPosition() - prevPosBars.x, 0));
}

void ExtendedCanvas::verticalScroll(wxScrollEvent &event) {
    doScroll(wxPoint(0, prevPosBars.y - event.GetPosition()));
}

void
ExtendedCanvas::toggleOption(CanvasItem *item, ict::ItemOption option) {
    if(!item) return;
    switch(option) {
        case ict::IOPT_LOCK:
            item->lock(!item->isLocked());
            break;
        case ict::IOPT_HIDE:
            item->hide(!item->isHidden());
            break;
        case ict::IOPT_SELECT:
            item->select(!item->isSelected());
            break;
        case ict::IOPT_RESTRICT:
            item->restrict(!item->isRestricted());
            break;
        case ict::IOPT_FIXEDASPECTRATIO:
            item->fixedAspectRatio(!item->fixedAspectRatio());
            break;
        case ict::IOPT_EXPANDFROMCENTER:
            item->expandFromCenter(!item->expandFromCenter());
            break;
        default:
            break;
    }
}

void ExtendedCanvas::keyDown(wxKeyEvent &event) {
    switch(event.GetKeyCode()) {
        case WXK_CONTROL:
            ctrlPressed = true;
            toggleOption(pressedItem, ict::IOPT_FIXEDASPECTRATIO);
            break;
        case WXK_SHIFT:
            shiftPressed = true;
            toggleOption(pressedItem, ict::IOPT_EXPANDFROMCENTER);
            break;
    }

    event.Skip();
}

void ExtendedCanvas::keyUp(wxKeyEvent &event) {
    switch(event.GetKeyCode()) {
        case WXK_CONTROL:
            ctrlPressed = false;
            toggleOption(pressedItem, ict::IOPT_FIXEDASPECTRATIO);
            break;
        case WXK_SHIFT:
            shiftPressed = false;
            toggleOption(pressedItem, ict::IOPT_EXPANDFROMCENTER);
            break;
    }

    event.Skip();
}

void ExtendedCanvas::checkModKeys() {
    if(!pressedItem) return;
    if(ctrlPressed) toggleOption(pressedItem, ict::IOPT_FIXEDASPECTRATIO);
    if(shiftPressed) toggleOption(pressedItem, ict::IOPT_EXPANDFROMCENTER);
}

void ExtendedCanvas::gridToggle(wxMouseEvent &event) {
    useGrid(!grid);
    event.Skip();
}

void ExtendedCanvas::mouseWheel(wxMouseEvent &event) {
    if(pressedItem) {
        event.Skip();
        return;
    }
    if(ctrlPressed) {
        wxDouble plus = event.GetWheelRotation();
        plus /= 1000;
        wxDouble sx;
        scaler->getNewFactor(&sx, nullptr);
        plus *= sx;
        scaler->plusFactor(plus, plus);
        doMagnify(event.GetPosition());
        scaler->getNewFactor(&sx, nullptr);
        zoom->setCustom(sx);
    } else {
        wxPoint motion(0, 0);
        if(event.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL) {
            if(shiftPressed) motion.x = event.GetWheelRotation() / 5;
            else motion.y = event.GetWheelRotation() / 5;
        } else {
            motion.x = event.GetWheelRotation() / 5;
        }
        doScroll(motion);
    }
    event.Skip();
}

void ExtendedCanvas::resizeCanvas(wxSizeEvent &event) {
    if (canvasBuffer) delete canvasBuffer;
    if(event.GetSize().x == 0 || event.GetSize().y == 0) return;
    canvasBuffer = new wxBitmap(event.GetSize().GetWidth(), event.GetSize().GetHeight());
    refreshCanvas();
    adjustScrollbars();
}

void ExtendedCanvas::mouseMotion(wxMouseEvent &event) {
    if(pressedItem) pressedItem->modify(event.GetPosition());
    else hoverCanvas(event.GetPosition());
    event.Skip();
}

bool ExtendedCanvas::hoverCanvas(const wxPoint p) {
    int collision = ict::NONE_ZONE;
    for(std::vector<CanvasItem *>::reverse_iterator it = zOrder.rbegin();
            it != zOrder.rend(); it++) {
        if(!collision) {
            collision = (*it)->inHandle(p);
            if((*it)->doHover(collision)) notifyHover(*it);
        } else if((*it)->doHover(ict::NONE_ZONE)) notifyHover(*it);
    }
    return collision;
}

bool ExtendedCanvas::pressCanvas(const wxPoint p) {
    for(std::vector<CanvasItem *>::reverse_iterator it = zOrder.rbegin();
            it != zOrder.rend(); it++) {
        if((*it)->press(p)) {
            pressedItem = *it;
            notifyPressure(pressedItem);
            checkModKeys();
            pressedItem->select(true);
            return true;
        }
    }
    if(selectedItem) selectedItem->select(false);
    return false;
}

void ExtendedCanvas::mousePress(wxMouseEvent &event) {
    if(!canvas->HasCapture()) canvas->CaptureMouse();
    pressCanvas(event.GetPosition());
    event.Skip();
}

void ExtendedCanvas::mouseRelease(wxMouseEvent &event) {
    if(canvas->HasCapture()) canvas->ReleaseMouse();
    if(pressedItem) {
        pressedItem->release();
        notifyPressure(pressedItem);
        checkModKeys();
        pressedItem = nullptr;
    }
    event.Skip();
}

void ExtendedCanvas::paintCanvas(wxPaintEvent &event) {
    if (!canvasBuffer) return;
    wxPaintDC canvasPainter(canvas);
    wxRegion damaged = canvas->GetUpdateRegion();
    wxRect toPaint(damaged.GetBox());
    {
    wxMemoryDC bufferPainter(*canvasBuffer);
    bufferPainter.SetBrush(*wxBLACK_BRUSH);
    bufferPainter.DrawRectangle(toPaint);
    for (std::vector<CanvasItem *>::iterator it = zOrder.begin();
            it != zOrder.end(); it++) {
        (*it)->drawOn(&bufferPainter);
    }
    }
    canvasPainter.DrawBitmap(*canvasBuffer, 0, 0);
}

CanvasItem * ExtendedCanvas::getItem(int itemId) {
    for (std::vector<CanvasItem *>::iterator it = zOrder.begin();
            it != zOrder.end(); it++) {
        if ((*it)->getId() == itemId) return *it;
    }
    return nullptr;
}

void ExtendedCanvas::addItem(CanvasItem *item) {
    if (!item) return;
    if (getItem(item->getId())) return;
    item->setScaler(scaler);
    item->setContainer(this);
    item->useGrid(grid);
    zOrder.push_back(item);
    wxRect2DDouble fresh(item->getArea());
    refreshCanvasRect(wxRect(fresh.m_x, fresh.m_y, fresh.m_width, fresh.m_height));
    adjustScrollbars();
}

void ExtendedCanvas::doMagnify(const wxPoint mousePosition) {
    if (scaler && !scaler->hasTransfer()) return;
    if (mousePosition.x < 0) return;
    if (mousePosition.y < 0) return;

    double of, tf, nf;
    scaler->getOldFactor(&of, nullptr);
    scaler->getTransferFactor(&tf, nullptr);
    scaler->getNewFactor(&nf, nullptr);
    canvasReference.m_x *= of;
    canvasReference.m_y *= of;
    canvasReference.m_x -= mousePosition.x;
    canvasReference.m_y -= mousePosition.y;
    canvasReference.m_x *= tf;
    canvasReference.m_y *= tf;
    canvasReference.m_x += mousePosition.x;
    canvasReference.m_y += mousePosition.y;
    canvasReference.m_x /= nf;
    canvasReference.m_y /= nf;

    scaler->clearTransfer();
    refreshCanvas();
    adjustScrollbars();
}

void ExtendedCanvas::doScroll(const wxPoint motion) {
    if (motion.x == 0 && motion.y == 0) return;

    canvasReference.m_x -= scaler->scaleX(motion.x, ict::OUT_D);
    canvasReference.m_y += scaler->scaleY(motion.y, ict::OUT_D);

    refreshCanvas();
    adjustScrollbars();
}

void ExtendedCanvas::adjustScrollbars() {
    if(!hasItems()) {
        initScrollbars();
        return;
    }
    wxRect coverage(getItemsCoverage());
    wxRect slideWin(0, 0, 1, 1);

    slideWin.x = canvas->GetSize().GetWidth() - coverage.GetWidth();
    int thumbSize;
    if(slideWin.x < 0) {
        thumbSize = canvas->GetSize().GetWidth();
        slideWin.width = abs(slideWin.x);
    } else {
        thumbSize = coverage.GetWidth();
        slideWin.x /= 2;
    }
    int thumbPos = 0;
    if(coverage.GetLeft() < slideWin.GetRight()) {
        thumbPos = slideWin.GetRight() - coverage.GetLeft();
    }
    int range = coverage.GetWidth();
    if(coverage.GetLeft() < slideWin.GetLeft()) {
        range += slideWin.GetLeft() - coverage.GetLeft();
    } else if(coverage.GetLeft() > slideWin.GetRight()) {
        range += coverage.GetLeft() - slideWin.GetRight();
    }
    hBar->SetScrollbar(thumbPos, thumbSize, range, thumbSize);
    prevPosBars.x = thumbPos;

    slideWin.y = canvas->GetSize().GetHeight() - coverage.GetHeight();
    if(slideWin.y < 0) {
        thumbSize = canvas->GetSize().GetHeight();
        slideWin.height = abs(slideWin.y);
    } else {
        thumbSize = coverage.GetHeight();
        slideWin.y /= 2;
    }
    thumbPos = 0;
    if(coverage.GetTop() < slideWin.GetBottom()) {
        thumbPos = slideWin.GetBottom() - coverage.GetTop();
    }
    range = coverage.GetHeight();
    if(coverage.GetTop() < slideWin.GetTop()) {
        range += slideWin.GetTop() - coverage.GetTop();
    } else if(coverage.GetTop() > slideWin.GetBottom()) {
        range += coverage.GetTop() - slideWin.GetBottom();
    }
    vBar->SetScrollbar(thumbPos, thumbSize, range, thumbSize);
    prevPosBars.y = thumbPos;
}

wxRect ExtendedCanvas::getItemsCoverage() {
    if(zOrder.empty()) return wxRect(0, 0, 1, 1);
    wxRect2DDouble ret(zOrder.front()->getArea());
    for(std::vector<CanvasItem *>::const_iterator it = zOrder.cbegin();
            it != zOrder.cend(); it++) {
        ret = ret.CreateUnion((*it)->getArea());
    }
    return wxRect(ret.m_x, ret.m_y, ret.m_width, ret.m_height);
}

void ExtendedCanvas::refreshCanvasRect(const wxRect &r) {
    canvas->RefreshRect(r);
}

void ExtendedCanvas::refreshCanvas() {
    canvas->Refresh();
}

void ExtendedCanvas::notifyGeometry(CanvasItem *changed) {
    wxRect2DDouble ch(changed->getAreaUpdate());
    wxRect refresh(ch.m_x, ch.m_y, ch.m_width, ch.m_height);
    refreshCanvasRect(refresh.Inflate(1, 1));
    adjustScrollbars();
    // send geometry event
}

void ExtendedCanvas::notifySelection(CanvasItem *changed) {
    if(!changed) return;
    if(selectedItem) {
        if(*selectedItem != *changed && changed->isSelected()) {
            selectedItem->select(false);
            selectedItem = changed;
        } else if(*selectedItem == *changed && !changed->isSelected())
            selectedItem = nullptr;
    } else if(changed->isSelected()) selectedItem = changed;
    wxRect2DDouble ch(changed->getArea());
    wxRect refresh(ch.m_x, ch.m_y, ch.m_width, ch.m_height);
    refreshCanvasRect(refresh);
    // send selection event
}

void ExtendedCanvas::notifyPressure(CanvasItem *pressed) {
    // send pressure event
}

void ExtendedCanvas::notifyHover(CanvasItem *changed) {
    wxRect2DDouble ch(changed->getHoverUpdate());
    wxRect refresh(ch.m_x, ch.m_y, ch.m_width, ch.m_height);
    refreshCanvasRect(refresh);
    // send hover event
}

wxPoint2DDouble ExtendedCanvas::getReference(ict::ECContext c) const {
    if(c == ict::VIRTUAL_CONTEXT) return canvasReference;
    else return scaler->scalePoint(canvasReference, ict::IN_D);
}

void ExtendedCanvas::useGrid(bool op) {
    if(grid == op) return;
    grid = op;
    for(std::vector<CanvasItem *>::reverse_iterator it = zOrder.rbegin();
            it != zOrder.rend(); it++) {
        (*it)->useGrid(grid);
    }
    refreshCanvas();
}

bool ExtendedCanvas::useGrid() const {
    return grid;
}

bool ExtendedCanvas::hasItems() const {
    return !zOrder.empty();
}

ExtendedCanvas::~ExtendedCanvas() {
    if (canvasBuffer) delete canvasBuffer;
}
