#include "imgtools.h"
#include <cmath>
#include <cstdlib>

using Magick::Quantum;

void overlap(const Magick::Image &overlay, Magick::Image &background, bool fit, 
        bool crop) {
    if(fit) {
        Magick::Geometry geoBack(background.columns(), background.rows());
        float backRatio = (float)background.columns() / (float)background.rows();
        float overRatio = (float)overlay.columns() / (float)overlay.rows();
        if(backRatio > overRatio) {
            geoBack.height(overlay.rows());
            geoBack.width((float)overlay.rows() * backRatio);
        } else {
            geoBack.width(overlay.columns());
            geoBack.height((float)overlay.columns() / backRatio);
        }
        background.resize(geoBack);
        if(crop) {
            int xco = (background.columns() - overlay.columns()) / 2;
            int yco = (background.rows() - overlay.rows()) / 2;
            background.crop(Magick::Geometry(overlay.columns(), overlay.rows(), xco, yco));
        }
    }
    background.composite(overlay, Magick::CenterGravity, 
            Magick::OverCompositeOp);
}

Magick::Image extractArea(const Magick::Geometry area, Magick::Image target) {
    Magick::Image extracted(area, Magick::Color(0, 0, 0, QuantumRange));
    if(emptyIntersection(area, target)) return extracted;
    target.crop(area);

    int xOff, yOff;
    if(area.xOff() < 0) xOff = abs(area.xOff());
    else xOff = 0;
    if(area.yOff() < 0) yOff = abs(area.yOff());
    else yOff = 0;

    Magick::Geometry compOff(0, 0, xOff, yOff);
    extracted.composite(target, compOff, Magick::OverCompositeOp);

    return extracted;
}

bool emptyIntersection(const Magick::Geometry &area, const Magick::Image &image) {
    bool outX = false, outY = false;
    outX = area.xOff() >= image.columns();
    if(area.xOff() < 0) outX = abs(area.xOff()) >= area.width();
    outY = area.yOff() >= image.rows();
    if(area.yOff() < 0) outY = abs(area.yOff()) >= area.height();
    return outX || outY;
}

bool contains(const Magick::Geometry &area, const Magick::Image &image) {
    if((area.xOff() <= 0) && (area.yOff() <= 0)) {
        int right = area.width() + area.xOff();
        int down = area.height() + area.yOff();
        if((right >= image.columns()) && (down >= image.rows())) {
            return true;
        }
    }
    return false;
}

unsigned char * extractDepth8Channel(Magick::Image &img, ict::Channel ch, bool opaqueAtHigh) {
    img.modifyImage();
    Magick::Pixels cache(img);
    Magick::PixelPacket *pixels = cache.get(0, 0, img.columns(), img.rows());
    int n = img.columns() * img.rows();
    unsigned char *pExt;
    switch(ch) {
    case ict::RGB:
        pExt = new unsigned char[n*3];
#if MAGICKCORE_QUANTUM_DEPTH == 16
        for(int p = 0; p < n; p++) {
            int offset = p * 3;
            pExt[offset] = toDepth8(pixels[p].red);
            offset++;
            pExt[offset] = toDepth8(pixels[p].green);
            offset++;
            pExt[offset] = toDepth8(pixels[p].blue);
        }
#elif MAGICKCORE_QUANTUM_DEPTH == 8
        for(int p = 0; p < n; p++) {
            int offset = p * 3;
            pExt[offset] = pixels[p].red;
            offset++;
            pExt[offset] = pixels[p].green;
            offset++;
            pExt[offset] = pixels[p].blue;
        }
#endif
        break;
    case ict::ALPHA:
        pExt = new unsigned char[n];
#if MAGICKCORE_QUANTUM_DEPTH == 16
        if(opaqueAtHigh) {
            for(int p = 0; p < n; p++) {
                pExt[p] = depth8 - toDepth8(pixels[p].opacity);
            }
        } else {
            for(int p = 0; p < n; p++) {
                pExt[p] = toDepth8(pixels[p].opacity);
            }
        }
#elif MAGICKCORE_QUANTUM_DEPTH == 8
        if(opaqueAtHigh) {
            for(int p = 0; p < n; p++) {
                pExt[p] = depth8 - pixels[ṕ].opacity;
            }
        } else {
            for(int p = 0; p < n; p++) {
                pExt[p] = pixels[p].opacity;
            }
        }
#endif
        break;
    }
    return pExt;
}

unsigned short toDepth16(const unsigned char value) {
    unsigned short n = value * depthFactor;
    return n;
}

unsigned char toDepth8(const unsigned short value) {
    unsigned char n = value / depthFactor;
    return n;
}

