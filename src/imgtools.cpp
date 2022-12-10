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

Magick::Image extractArea(const Magick::Geometry &area, const Magick::Image &target) {
    Magick::Image extracted(area, Magick::Color(0, 0, 0, QuantumRange));
    Magick::Image tCopy(target);
    if(emptyIntersection(area, tCopy)) return extracted;
    tCopy.crop(area);

    int xOff, yOff;
    if(area.xOff() < 0) xOff = abs(area.xOff());
    else xOff = 0;
    if(area.yOff() < 0) yOff = abs(area.yOff());
    else yOff = 0;

    Magick::Geometry compOff(0, 0, xOff, yOff);
    extracted.composite(tCopy, compOff, Magick::OverCompositeOp);

    return extracted;
}

bool emptyIntersection(const Magick::Geometry &area, const Magick::Image &image) {
    bool mtX = false, mtY = false;
    mtX = area.xOff() >= image.columns();
    if(area.xOff() < 0) mtX = abs(area.xOff()) >= area.width();
    mtY = area.yOff() >= image.rows();
    if(area.yOff() < 0) mtY = abs(area.yOff()) >= area.height();
    return mtX || mtY;
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

unsigned char * extractDepth8Channel(const Magick::Image &img, ict::Channel ch, bool opaqueAtHigh) {
    Magick::Image iCopy(img);
    iCopy.modifyImage();
    Magick::Pixels cache(iCopy);
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

Magick::Image composeState(const Magick::Image &img, const State &s) {
    int w = std::get<1>(s).cropSize.GetWidth();
    int h = std::get<1>(s).cropSize.GetHeight();
    int xo = std::get<0>(s).x;
    int yo = std::get<0>(s).y;
    Magick::Geometry crop(w, h, xo, yo);
    Magick::Image comp(extractArea(crop, img));
    if(std::get<1>(s).allowGrow) {
        //Magick::Image back(img.geometry(), Magick::Color(0, 0, 0, QuantumRange / 2));
        //if(std::get<1>(s).growChoice == ict::IMAGE) 
        //    if(!std::get<1>(s).backImage.ToStdString().empty())
        //        back.read(std::get<1>(s).backImage.ToStdString());
        //overlap(comp, back, true, true);
        //return back;
    }
    return comp;
}

wxImage createImage(const Magick::Image &img) {
    return wxImage(img.columns(), img.rows(), 
            extractDepth8Channel(img, ict::RGB), extractDepth8Channel(img, ict::ALPHA));
}

