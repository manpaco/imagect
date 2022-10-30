#include "composer.h"
#include "Magick++/Color.h"
#include "Magick++/Geometry.h"
#include "Magick++/Image.h"
#include "Magick++/Include.h"
#include "magick/magick-type.h"

using Magick::Quantum;

Composer::Composer() {
    this->cropBack = true;
    this->fitBack = true;
}

Composer::Composer(const bool cropBack, const bool fitBack) {
    cropBackground(cropBack);
    fitBackground(fitBack);
}

void Composer::overlap(const Magick::Image &overlay, Magick::Image &background) {
    if(fitBackground()) {
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

    if(cropBackground()) {
        background.crop(Magick::Geometry(overlay.columns(), overlay.rows()));
    }

    background.composite(overlay, Magick::CenterGravity, Magick::OverCompositeOp);
}

Magick::Image Composer::extractArea(const Magick::Geometry area, Magick::Image target) {
    Magick::Image extracted(area, Magick::Color(0, 0, 0, QuantumRange));
    target.crop(area);

    Magick::Geometry compOff(0, 0, abs(area.xOff()), abs(area.yOff()));
    extracted.composite(target, compOff, Magick::OverCompositeOp);

    return extracted;
}

bool Composer::isContained(const Magick::Geometry &area, const Magick::Image &image) const {
    if((area.xOff() <= 0) && (area.yOff() <= 0)) {
        int right = area.width() + area.xOff();
        int down = area.height() + area.yOff();
        if((right >= image.columns()) && (down >= image.rows())) {
            return true;
        }
    }
    return false;
}

void Composer::cropBackground(const bool option) {
    this->cropBack = option;
}

void Composer::fitBackground(const bool option) {
    this->fitBack = option;
}

bool Composer::cropBackground() const {
    return cropBack;
}

bool Composer::fitBackground() const {
    return fitBack;
}
