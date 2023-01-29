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

#ifndef ZOOMEVENT_H
#define ZOOMEVENT_H

#include "wx/event.h"

/**
 * Implementation of a zoom event
 *
 * Contains information about zoom value.
 */
class ZoomEvent : public wxEvent {
    public:
        ZoomEvent(wxEventType, int, float);
        float getScaleFactor() const;
        virtual wxEvent * Clone() const;

    private:
        float scaleFactor;
};

wxDECLARE_EVENT(EVT_ZOOM_CHANGE, ZoomEvent);

#endif // ZOOMEVENT_H
