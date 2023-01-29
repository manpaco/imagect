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

#ifndef ZOOMCTRL_H
#define ZOOMCTRL_H

class wxButton;
class wxTextCtrl;

#include "wx/control.h"
#include "defs.h"

static const ict::ZoomArray zValues = {
    0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0
};

/**
 * Implementation of a zoom control
 *
 * Manages the zoom changes sending events.
 * Offers buttons and display box.
 */
class ZoomCtrl : public wxControl {
    public:
        ZoomCtrl(wxWindow *parent, wxWindowID id);
        ~ZoomCtrl();

    private:
        void onZout(wxCommandEvent &event);
        void onZin(wxCommandEvent &event);
        void sendZoomEvent();
        void showPercent(float sf);

        ict::ZoomArray::const_iterator zIt;
        wxButton *zIn, *zOut;
        wxTextCtrl *percent;
};

#endif // ZOOMCTRL_H
