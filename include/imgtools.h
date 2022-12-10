#ifndef IMGTOOLS_H
#define IMGTOOLS_H

#include <Magick++.h>
#include "identifiersdef.h"
#include "statedef.h"

const unsigned char depth8 = 255;
const unsigned short depth16 = 65535;
const unsigned short depthFactor = depth8 + 2;

void overlap(const Magick::Image &, Magick::Image &, bool, bool);
Magick::Image extractArea(const Magick::Geometry &, const Magick::Image &);
bool emptyIntersection(const Magick::Geometry &area, const Magick::Image &image);
bool contains(const Magick::Geometry &, const Magick::Image &);
unsigned char * extractDepth8Channel(const Magick::Image &, ict::Channel, bool = true);
unsigned char toDepth8(const unsigned short);
unsigned short toDepth16(const unsigned char);
Magick::Image composeState(const Magick::Image &img, const State &s);
wxImage createImage(const Magick::Image &img);

#endif // IMGTOOLS_H
