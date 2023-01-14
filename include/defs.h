#ifndef DEFS_H
#define DEFS_H

#include "wx/defs.h"

namespace ict {

const int IMG_MULTIPLIER = 3;
const int SHADOW_OFFSET = 2;
const int CORNER = 10;
const unsigned int MIN_CROP = 1;
const unsigned int MIN_STROKE = 0;

enum ControlID {
    WIDTH_TC = wxID_HIGHEST + 1,
    HEIGHT_TC,
    FIX_RATIO_CB,

    GROW_CHECK_CB,
    GROW_SELECTOR_CH,
    PICK_COLOUR_BT,
    BACK_BLUR_SL,
    PICK_IMG_FP,

    SHAPE_CH,
    STROKE_WIDTH_TC,
    PICK_STROKE_COLOUR_BT,

    APPLY_BT,
    RESET_CROP_BT,
    EXPORT_MI,

    CROP_AREA

};

enum PanelID {
    CANVAS = wxID_HIGHEST + 100,
    SCVIEW,
    TOOLS,
    PREVIEW
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
    VOID_G,
    GROW_CHOICE_SIZE
};

enum ShapeChoice {
    SQUARE = 0,
    CIRCLE,
    TRIANGLE,
    SHAPE_CHOICE_SIZE
};

enum Zone {
    NONE = 0,
    INNER,
    N,
    NE,
    NW,
    S,
    SE,
    SW,
    E,
    W
};

enum Channel {
    RGB,
    ALPHA
};

enum Depth {
    DEPTH8,
    DEPTH16
};

enum Wildcard {
    PNG_WC = 0,
    JPEG_WC,
    WILDCARD_SIZE
};

enum Extension {
    NONE_EXT = 0,
    PNG_EXT,
    JPG_EXT,
    JPEG_EXT
};

enum Tot {
    SCALE_T = 0,
    COMPRESS_T,
    SNC_T
};

enum Dot {
    IN_D = 0,
    OUT_D
};

}

#endif // DEFS_H
