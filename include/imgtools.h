#ifndef IMGTOOLS_H
#define IMGTOOLS_H

#include <Magick++.h>

void overlap(const Magick::Image &overlay, Magick::Image &background);
Magick::Image extractArea(const Magick::Geometry area, const Magick::Image target);
bool isContained(const Magick::Geometry &area, const Magick::Image &image);
unsigned char * extractRgb(Magick::Image &img);
unsigned char * extractAlpha(Magick::Image &img);
unsigned char convertValue(unsigned short value);

#endif // IMGTOOLS_H
