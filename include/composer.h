#ifndef COMPOSER_H
#define COMPOSER_H

#include "Magick++/Geometry.h"
#include "Magick++/Image.h"

class Composer {

    public:
        Composer();
        Composer(const bool cropBack, const bool fitBack);
        void overlap(const Magick::Image &overlay, Magick::Image &background);
        Magick::Image extractArea(const Magick::Geometry area, const Magick::Image target);
        void cropBackground(const bool option);
        void fitBackground(const bool option);
        bool cropBackground() const;
        bool fitBackground() const;
        bool isContained(const Magick::Geometry &area, const Magick::Image &image) const;

    private:
        bool cropBack;
        bool fitBack;
};

#endif // COMPOSER_H
