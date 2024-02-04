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

#ifndef EXTENDEDCANVAS_H
#define EXTENDEDCANVAS_H

#include <vector>
#include <wx/event.h>
#include <wx/window.h>
#include <wx/geometry.h>

class wxScrollBar;
class wxFlexGridSizer;
class wxPanel;
class CanvasItem;
class Scaler;

class ExtendedCanvas : public wxWindow {
public:
    ExtendedCanvas(wxWindow *parent, wxWindowID id);

    void addItem(CanvasItem *item);
    CanvasItem * getItem(int itemId);

    ~ExtendedCanvas();

private:
    void paintCanvas(wxPaintEvent &event);
    void mouseMotion(wxMouseEvent &event);
    void mousePress(wxMouseEvent &event);
    void mouseRelease(wxMouseEvent &event);
    void canvasResize(wxSizeEvent &event);
    void magnification(wxMouseEvent &event);
    void doMagnify(const wxPoint magCenter);
    void doScroll(const wxPoint motion);
    CanvasItem * pressCanvas(const wxPoint p);
    void refreshCanvas();
    void refreshCanvasRect(const wxRect &r);

    wxScrollBar *vBar, *hBar;
    wxFlexGridSizer *layout;
    wxWindow *canvas;
    wxWindow *zoom;
    Scaler *scaler;
    std::vector<CanvasItem *> zOrder;
    CanvasItem *pressItem, *oldSelectedItem;
    wxPoint2DDouble canvasReference;

    wxBitmap *canvasBuffer;
};

#endif // !EXTENDEDCANVAS_H
