#ifndef DEFS_H
#define DEFS_H

#include "wx/defs.h"
#include <array>

namespace ict {

const double MAXUPP = 256;
const double MINUPP = 1 / MAXUPP;

const unsigned int MIN_CROP = 1;
const unsigned int MIN_STROKE = 0;
const int BEST_SPACE = 5;
const int zoomArraySize = 8;

typedef std::array<float, zoomArraySize> ZoomArray;

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

    ZIN_BT,
    ZOUT_BT,
    ZOOM_CT,

    CROP_AREA

};

enum PanelID {
    CANVAS = wxID_HIGHEST + 100,
    SCVIEW,
    TOOLS,
    PREVIEW
};

enum CollPaneID {
    ASPECT = wxID_HIGHEST + 200,
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
    VOID,
    GROW_CHOICE_SIZE
};

enum ShapeChoice {
    SQUARE = 0,
    CIRCLE,
    TRIANGLE,
    SHAPE_CHOICE_SIZE
};

enum Channel {
    RGB,
    ALPHA
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

enum Dot {
    IN_D = 0,
    OUT_D
};

enum TruncType {
    FLOOR_ST,
    ROUND_ST,
    CEILL_ST
};

enum ECContext {
    VIRTUAL_CONTEXT,
    CANVAS_CONTEXT
};

}

#endif // DEFS_H
