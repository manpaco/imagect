/**
 *     Copyright (C) 2023 Acuilán Gabriel <acuilangabriel@gmail.com>
 *
 *     This file is part of Image Cropping Tool.
 *
 *     Image Cropping Tool is free software: you can redistribute it and/or
 *     modify it under the terms of the GNU General Public License as published
 *     by the Free Software Foundation, either version 3 of the License, or (at
 *     your option) any later version.
 * 
 *     Image Cropping Tool is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 *     Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License along
 *     with Image Cropping Tool. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CANVASPANEL_H
#define CANVASPANEL_H

class wxGraphicsContext;
namespace Magick {
    class Image;
}

#include "wx/panel.h"
#include "controller.h"
#include "defs.h"

/**
 * Implementation of a canvas panel.
 *
 * Manages the painting process of a canvas, e.g. the crop rectangle using a 
 * controller, the centered image using a buffer, the cursors changes.
 * Support zoom and works with small buffer.
 * Use translations to transform dimension between controller and panel.
 * Communication between user inputs and controller.
 */
class CanvasPanel: public wxPanel {
    public:
        /**
         * Construct a canvas that handle a given image.
         */
        CanvasPanel(wxWindow *parent, wxWindowID id, Magick::Image *img);

        /**
         * Default Ctor.
         */
        CanvasPanel(wxWindow *parent, wxWindowID id);

        /**
         * Define the size of crop rectangle.
         * 
         * @param s Size to define. This is modified if necessary, e.g. applies
         *        restrictions.
         * @return true if the given size was defined; false if another size 
         *         was defined and given size was modified.
         */
        bool cropSize(wxSize *s);

        /**
         * Define the crop geometry (offset and size).
         * 
         * @param g Geometry to define. This is modified if necessary, e.g. 
         *        applies restrictions.
         * @return true if the given geometry was defined; false if another 
         *         geometry was defined and given geometry was modified.
         */
        bool cropGeometry(wxRect *g);

        /**
         * Get the crop geometry (offset and size).
         */
        wxRect cropGeometry() const;

        /**
         * Get the crop size.
         */
        wxSize cropSize() const;

        /**
         * Get the crop offset (relative to image).
         */
        wxPoint cropOffset() const;

        /**
         * Enable or disable the fix aspect ratio.
         */
        void fixCrop(bool);

        /**
         * Allow or deny growing.
         *
         * @return true if crop rectangle is modified after deny growing,
         *         else false.
         */
        bool allowGrow(bool);

        /**
         * Set the scale factor (zoom) and refresh the canvas.
         */
        void setScaleFactor(float sf);

        /**
         * Get the translation of the given rectangle.
         *
         * @param r Given rectangle.
         * @param t Type of translation.
         * @param d Direction of translation.
         */
        wxRect translateRect(const wxRect &r, ict::Tot t, ict::Dot d) const;

        /**
         * Get the translation of the given point.
         *
         * @param r Given point.
         * @param t Type of translation.
         * @param d Direction of translation.
         */
        wxPoint translatePoint(const wxPoint &p, ict::Tot t, ict::Dot d) const;
        
        /**
         * Get the translation of the given size.
         *
         * @param r Given size.
         * @param t Type of translation.
         * @param d Direction of translation.
         */
        wxSize translateSize(const wxSize &s, ict::Tot t, ict::Dot d) const;

        /**
         * Dtor.
         */
        ~CanvasPanel();

    private:
        /**
         * Paint the damaged area of the panel.
         */
        void onPaint(wxPaintEvent &event);

        /**
         * Emits a crop event with information about crop geometry.
         */
        void sendCropEvent();

        /**
         * Change de cursor pointer depending of the zone.
         */
        void changeCursor(ict::Zone type);

        /**
         * Get the translation of the given value. This use the scale and/or
         * compress factors to perform de translation.
         *
         * @param v Value to translate.
         * @param t Type of Translation.
         * @param d Direction of Translation.
         */
        int translate(int v, ict::Tot t, ict::Dot d) const;

        /**
         * Paint the crop rectangle with specific lines and colors.
         *
         * @param paint Rectangle used has frame.
         * @param gc Graphics context used to paint.
         */
        void paintCropRect(const wxRect &paint, wxGraphicsContext *gc);

        /**
         * Initialize the buffer with the given Bitmap.
         */
        void initBuffer(wxBitmap &);

        /**
         * Init buffer and refresh canvas.
         */
        void initCanvas(wxBitmap &);

        /**
         * Update the panel size based on the scale factor and buffer size.
         */
        void updateSizes();

        /**
         * Recalculate the zones and panel sizes.
         */
        void refreshCanvas();

        /**
         * Calculate the damaged area by doing union between previous and 
         * current crop rectangle, then emits a painting event. Also update
         * zones.
         */
        void refreshDamaged();

        /**
         * Get the shadow rectangle based on the compressed image.
         * 
         * @return image rectangle expanded by ict::SHADOW_OFFSET
         */
        wxRect shadowRect();

        /**
         * Manages the mouse motion. Changes cursor or modify controller
         * depending of the controller pressure state.
         */
        void mouseMotion(wxMouseEvent &event);

        /**
         * When the mouse emits a down button event, the controller is pressed.
         */
        void mousePress(wxMouseEvent &event);

        /**
         * When the mouse emits a up button event, the controller is released.
         */
        void mouseRelease(wxMouseEvent &event);

        /**
         * Transform the absolute coordinates of the given point to others
         * that are relative to image position.
         */
        wxPoint relativeToImage(const wxPoint &ap) const;

        /**
         * Transform the relative coordinates of the given point to others
         * that are relative to panel (absolute coordinates).
         */
        wxPoint absoluteCoords(const wxPoint &rp) const;

        /**
         * Compress the given image to reduce the final size of the buffer.
         */
        void compressImage(Magick::Image *);

        /**
         * Update the zones based in crop rectangle translated to panel.
         */
        void updateZones();

        /**
         * Get the zone which contains the point.
         */
        ict::Zone getLocation(const wxPoint &p) const;

        CropController controller;
        wxBitmap *buffer = nullptr;
        wxRect imgRect;
        wxSize bufferSize;
        float scaleFactor = 1.0, compressFactor = 1.0;

        wxRect iz, nz, sz, ez, wz, nez, nwz, sez, swz;
        wxPoint lastPoint;
        wxRect prevCrop;
};

#endif // CANVASPANEL_H
