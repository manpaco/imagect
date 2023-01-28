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

/**
 * Put a image on top of another.
 *
 * @param overlay Image used to put on the top.
 * @param background Image used as background.
 * @param fit If true the background is modified to fit overlay.
 * @param crop If true the background is croped using overlay dimensions.
 */
void overlap(const Magick::Image &, Magick::Image &, bool = true, bool = true);

/**
 * Extract a portion of a image.
 *
 * @param area Used to extract pixels.
 * @param target Pixels to be extracted.
 */
Magick::Image extractArea(const Magick::Geometry &, const Magick::Image &);

/**
 * Return true if the area not contains pixels of the image.
 */
bool emptyIntersection(const Magick::Geometry &area,
                       const Magick::Image &image);

/**
 * Return true if the image fully contains the area.
 */
bool contains(const Magick::Geometry &, const Magick::Image &);

/**
 * Return an depth 8 channel.
 *
 * @param img From where the channel is extracted.
 * @param ch Type of channel to extract.
 * @param opaqueAtHigh If true, then opaque is defined as max. value.
 */
unsigned char * extractDepth8Channel(const Magick::Image &,
                                     ict::Channel,
                                     bool = true);

/**
 * Transform the depth 16 value to depth 8.
 */
unsigned char toDepth8(const unsigned short);

/**
 * Transform the depth 8 value to depth 16.
 */
unsigned short toDepth16(const unsigned char);

/**
 * Compose the image with base image and options given.
 *
 * @param img Base image.
 * @param s Options to use.
 * @return the composed image.
 */
Magick::Image composeState(const Magick::Image &img,
                           const OptionsContainer &s);

/**
 * Transform the MagickImage to wxImage
 */
wxImage createImage(const Magick::Image &img);

/**
 * Try to open the image file given.
 *
 * @param imgFile File to try open.
 * @param msgTitle Title of the message if an error occurs.
 * @return true if fiel was opened succesful, else false.
 */
bool tryOpen(const wxString &imgFile, const wxString &msgTitle);

/**
 * Return the float factor to fit a geometry such that there is NO overflow in
 * the image.
 * Maintains the aspect ratio.
 */
float factorToFit(Magick::Geometry &g, Magick::Geometry &toFit);

/**
 * Return the float factor to fit a geometry such that there is overflow in the
 * image.
 * Maintains the aspect ratio.
 */
float factorToGrowFit(Magick::Geometry &area, Magick::Geometry &toFit);

#endif // IMGTOOLS_H
