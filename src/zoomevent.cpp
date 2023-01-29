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

#include "zoomevent.h"

wxDEFINE_EVENT(EVT_ZOOM_CHANGE, ZoomEvent);

ZoomEvent::
ZoomEvent(wxEventType eventType, int winId, float sf) :
    wxEvent(winId, eventType), scaleFactor(sf) {

}

float ZoomEvent::getScaleFactor() const {
    return scaleFactor;
}

wxEvent * ZoomEvent::Clone() const {
    return new ZoomEvent(*this);
}
