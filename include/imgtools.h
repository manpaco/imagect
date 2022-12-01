#ifndef IMGTOOLS_H
#define IMGTOOLS_H

#include <Magick++.h>

void overlap(const Magick::Image &, Magick::Image &, bool, bool);
Magick::Image extractArea(const Magick::Geometry, const Magick::Image);
bool isContained(const Magick::Geometry &area, const Magick::Image &image);
bool isFullyContained(const Magick::Geometry &, const Magick::Image &);
unsigned char * extractRgb(Magick::Image &);
unsigned char * extractAlpha(Magick::Image &);
unsigned char convertValue(unsigned short);

#endif // IMGTOOLS_H
