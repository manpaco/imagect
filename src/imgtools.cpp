#include "imgtools.h"

using Magick::Quantum;

void overlap(const Magick::Image &overlay, Magick::Image &background, bool fit, 
        bool crop) {
    if(fit) {
        Magick::Geometry geoBack(background.columns(), background.rows());
        geoBack.aspect(false);
        int xDiff = geoBack.width() - overlay.columns();
        int yDiff = geoBack.height() - overlay.rows();
        if(xDiff < yDiff){
            geoBack.width(overlay.columns());
        } else {
            geoBack.height(overlay.rows());
        }
        background.resize(geoBack);
    }

    if(crop) {
        background.crop(Magick::Geometry(overlay.columns(), overlay.rows()));
    }

    background.composite(overlay, Magick::CenterGravity, 
            Magick::OverCompositeOp);
}

Magick::Image extractArea(const Magick::Geometry area, Magick::Image target) {
    Magick::Image extracted(area, Magick::Color(0, 0, 0, QuantumRange));
    target.crop(area);

    Magick::Geometry compOff(0, 0, abs(area.xOff()), abs(area.yOff()));
    extracted.composite(target, compOff, Magick::OverCompositeOp);

    return extracted;
}

bool isContained(const Magick::Geometry &area, const Magick::Image &image) {
    if((area.xOff() <= 0) && (area.yOff() <= 0)) {
        int right = area.width() + area.xOff();
        int down = area.height() + area.yOff();
        if((right >= image.columns()) && (down >= image.rows())) {
            return true;
        }
    }
    return false;
}

unsigned char * extractRgb(Magick::Image &img) {
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

unsigned char * extractAlpha(Magick::Image &img) {

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

unsigned char convertValue(unsigned short value_16d) {
    value_16d = value_16d / 255;
    if(value_16d > 255) {
        return 255;
    } else {
        return value_16d;
    }
}
