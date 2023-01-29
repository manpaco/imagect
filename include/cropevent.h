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

#ifndef CROPEVENT_H
#define CROPEVENT_H

#include "wx/event.h"

/**
 * Implementation of a crop event
 *
 * Constains information about crop geometry.
 */
class CropEvent : public wxEvent {
    public:
        CropEvent(wxEventType, int, const wxSize &, wxPoint &o);
        wxSize getSize() const;
        wxPoint getOffset() const;
        virtual wxEvent * Clone() const;

    private:
        wxSize size;
        wxPoint offset;

};

wxDECLARE_EVENT(EVT_CROP_CHANGE, CropEvent);

#endif // RECTANGLEEVENT_H
