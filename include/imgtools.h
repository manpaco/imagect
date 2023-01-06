#ifndef IMGTOOLS_H
#define IMGTOOLS_H

class wxString;
class wxImage;
namespace Magick {
    class Image;
    class Geometry;
}
struct OptionsContainer;

#include "defs.h"

const unsigned char depth8 = 255;
const unsigned short depth16 = 65535;
const unsigned short depthFactor = depth8 + 2;

void overlap(const Magick::Image &, Magick::Image &, bool = true, bool = true);
Magick::Image extractArea(const Magick::Geometry &, const Magick::Image &);
bool emptyIntersection(const Magick::Geometry &area, const Magick::Image &image);
bool contains(const Magick::Geometry &, const Magick::Image &);
unsigned char * extractDepth8Channel(const Magick::Image &, ict::Channel, bool = true);
unsigned char toDepth8(const unsigned short);
unsigned short toDepth16(const unsigned char);
Magick::Image composeState(const Magick::Image &img, const OptionsContainer &s);
wxImage createImage(const Magick::Image &img);
bool tryOpen(const wxString &imgFile, const wxString &msgTitle);
float factorToFit(Magick::Geometry &g, Magick::Geometry &toFit);
float factorToGrowFit(Magick::Geometry &area, Magick::Geometry &toFit);

#endif // IMGTOOLS_H
