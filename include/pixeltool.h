#ifndef PIXELTOOL_H
#define PIXELTOOL_H

#include "Magick++/Image.h"

class PixelTool {
    public:
        PixelTool();
        unsigned char * extractRgb(Magick::Image &img);
        unsigned char * extractAlpha(Magick::Image &img);
        unsigned char convertValue(unsigned short value_16d);
        ~PixelTool();
};

#endif // PIXELTOOL_H
