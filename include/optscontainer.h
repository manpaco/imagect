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

#ifndef OPTSCONTAINER_H
#define OPTSCONTAINER_H

#include "wx/gdicmn.h"
#include "wx/colour.h"
#include "wx/string.h"

/**
 * Container to storage tool options.
 */
struct OptionsContainer {
    wxSize cropSize;
    wxPoint cropOff;
    bool fixRatio;
    float ratio;
    int shapeChoice;
    unsigned int strokeWidth;
    wxColour strokeColour;
    int strokeOff;
    bool allowGrow;
    int growChoice;
    wxColour backColour;
    wxString backImage;
    int backBlur;

    OptionsContainer() : 
        cropSize(1, 1),
        cropOff(0, 0),
        fixRatio(false),
        ratio(0.0),
        shapeChoice(0),
        strokeWidth(0),
        strokeColour(*wxBLACK),
        strokeOff(0),
        allowGrow(false),
        growChoice(0),
        backColour(*wxBLACK),
        backImage(""),
        backBlur(0) {

    }

    bool operator==(const OptionsContainer &oc) const {
        return cropSize == oc.cropSize &&
            cropOff == oc.cropOff &&
            fixRatio == oc.fixRatio &&
            shapeChoice == oc.shapeChoice &&
            strokeWidth == oc.strokeWidth &&
            strokeColour == oc.strokeColour &&
            strokeOff == oc.strokeOff &&
            allowGrow == oc.allowGrow &&
            growChoice == oc.growChoice &&
            backColour == oc.backColour &&
            backImage == oc.backImage &&
            backBlur == oc.backBlur;
    }

    bool operator!=(const OptionsContainer &oc) const {
        return !(*this == oc);
    }
};

#endif // OPTSCONTAINER_H
