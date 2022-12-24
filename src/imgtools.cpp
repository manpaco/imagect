#include "imgtools.h"
#include <cmath>
#include <cstdlib>
#include <Magick++.h>
#include "optscontainer.h"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

using Magick::Quantum;

void overlap(const Magick::Image &overlay, Magick::Image &background, bool fit, 
        bool crop) {
    Magick::Geometry geoBack(background.columns(), background.rows());
    Magick::Geometry geoOver(overlay.columns(), overlay.rows());
    if(fit && (geoOver != geoBack)) {
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

    int xOff = 0, yOff = 0;
    if(area.xOff() < 0) xOff = abs(area.xOff());
    if(area.yOff() < 0) yOff = abs(area.yOff());

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
        case ict::RGB: {
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
#else
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
        }
        case ict::ALPHA: {
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
#else
            if(opaqueAtHigh) {
                for(int p = 0; p < n; p++) {
                    pExt[p] = depth8 - pixels[p].opacity;
                }
            } else {
                for(int p = 0; p < n; p++) {
                    pExt[p] = pixels[p].opacity;
                }
            }
#endif
            break;
        }
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

bool tryOpen(const wxString &imgFile, const wxString &msgTitle) {
    try {
        if(imgFile.IsEmpty()) throw std::invalid_argument("No image selected! Please, select a valid one to continue...");
        Magick::Image test(imgFile.ToStdString());
    }
    catch(Magick::Exception &e) {
        wxMessageBox(e.what(), msgTitle);
        return false;
    }
    catch(std::invalid_argument &a) {
        wxMessageBox(a.what(), msgTitle);
        return false;
    }
    return true;
}

Magick::Image composeState(const Magick::Image &img, const OptionsContainer &s) {
    int w = s.cropSize.GetWidth();
    int h = s.cropSize.GetHeight();
    int xo = s.cropOff.x;
    int yo = s.cropOff.y;
    Magick::Geometry crop(w, h, xo, yo);
    Magick::Image comp(extractArea(crop, img));
    if(s.allowGrow) {
        switch(s.growChoice) {
            case ict::IMAGE: {
                try {
                    Magick::Image back(s.backImage.ToStdString());
                    overlap(comp, back);
                    return back;
                }
                catch(Magick::Exception &e) {
                    std::cerr << e.what() << std::endl;
                }
                break;
            }
            case ict::COLOR: {
                wxColour c = s.backColour;
#if MAGICKCORE_QUANTUM_DEPTH == 16
                unsigned short r = toDepth16(c.Red());
                unsigned short g = toDepth16(c.Green());
                unsigned short b = toDepth16(c.Blue());
#else
                unsigned char r = c.Red();
                unsigned char g = c.Green();
                unsigned char b = c.Blue();
#endif
                Magick::Color bc(r, g, b);
                Magick::Image back(Magick::Geometry(comp.columns(), comp.rows()), bc);
                overlap(comp, back);
                return back;
                break;
            }
        }
    }
    return comp;
}

wxImage createImage(const Magick::Image &img) {
    return wxImage(img.columns(), img.rows(), 
            extractDepth8Channel(img, ict::RGB), extractDepth8Channel(img, ict::ALPHA));
}

