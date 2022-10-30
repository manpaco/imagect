#include "pixeltool.h"
#include "Magick++/Image.h"
#include "Magick++/Include.h"
#include "Magick++/Pixels.h"
#include <iostream>

PixelTool::PixelTool() {

}

unsigned char * PixelTool::extractRgb(Magick::Image &img) {
    img.modifyImage();
    Magick::Pixels cache(img);
    Magick::PixelPacket *pixels = cache.get(0, 0, img.columns(), img.rows());
    int n = img.columns() * img.rows();
    unsigned char *pExt = new unsigned char[n*3];
    for(int p = 0; p < n; p++) {
        int offset = p * 3;
        *(pExt + offset) = convertValue((pixels + p)->red);
        offset++;
        *(pExt + offset) = convertValue((pixels + p)->green);
        offset++;
        *(pExt + offset) = convertValue((pixels + p)->blue);
    }
    return pExt;
};

unsigned char * PixelTool::extractAlpha(Magick::Image &img) {

    img.modifyImage();
    Magick::Pixels cache(img);
    Magick::PixelPacket *pixels = cache.get(0, 0, img.columns(), img.rows());
    int n = img.columns() * img.rows();
    unsigned char *pExt = new unsigned char[n];
    for(int p = 0; p < n; p++) {
        *(pExt + p) = 255 - convertValue((pixels + p)->opacity);
    }
    return pExt;
}

unsigned char PixelTool::convertValue(unsigned short value_16d) {
    value_16d = value_16d / 255;
    if(value_16d > 255) {
        return 255;
    } else {
        return value_16d;
    }
}

PixelTool::~PixelTool() {

}

