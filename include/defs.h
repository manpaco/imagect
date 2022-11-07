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
    COLOR_PICKER_BT,
    BACK_BLUR_SL,
    PICK_BACK_FP,

    SHAPE_CH,

    APPLY_BT

};

enum PanelID {
    CANVAS = wxID_HIGHEST + 100,
    TOOLS,
    PREVIEW,
};

enum CollPaneID {
    ASPECT_RATIO = wxID_HIGHEST + 200,
    GROW,
    SHAPE
};

enum FrameID {
    MAIN = wxID_HIGHEST + 300,
};

enum SplitterID {
    MAIN_SPLITTER = wxID_HIGHEST +400,
    SIDE_SPLITTER
};

enum GrowChoice {
    COLOR = 0,
    IMAGE,
    GROW_CHOICE_SIZE
};

enum ShapeChoice {
    SQUARE = 0,
    CIRCLE,
    TRIANGLE,
    SHAPE_CHOICE_SIZE
};

};

#endif // DEFS_H
