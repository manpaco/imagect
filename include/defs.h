#ifndef DEFS_H
#define DEFS_H

#include "wx/defs.h"

namespace ict {

enum ControlID {
    WIDTH_TC = wxID_HIGHEST + 1,
    HEIGHT_TC,
    FIX_RATIO_CB,

    GROW_CHECK_CB,
    GROW_SELECTOR_RB,
    BACK_BLUR_SL,
    COLOR_PICKER_BT,

    SHAPE_CH,

    SHOW_PREVIEW_BT
};

enum PanelID {
    CANVAS = wxID_HIGHEST + 100,
    TOOLS,
    SPLITTER
};

enum CollPaneID {
    ASPECT_RATIO = wxID_HIGHEST + 200,
    GROW,
    SHAPE
};

enum FrameID {
    MAIN = wxID_HIGHEST + 300,
    PREVIEW
};

enum GrowChoice {
    COLOR = 0,
    IMAGE,
    GROW_CHOICE_SIZE
};

enum ShapeChoice {
    SQUARE = 0,
    CIRCULE,
    TRIANGLE,
    SHAPE_CHOICE_SIZE
};

};

#endif // DEFS_H
