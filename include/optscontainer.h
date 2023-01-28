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
