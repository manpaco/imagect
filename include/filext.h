#ifndef FILEXT_H
#define FILEXT_H

#include <string>
#include "defs.h"

const std::string pngExt = ".png";
const std::string jpgExt = ".jpg";
const std::string jpegExt = ".jpeg";
const std::string pngWc = "PNG image (*" + pngExt + ")|*" + pngExt;
const std::string jpegWc = "JPEG image (*" + jpgExt +  ";*" + jpegExt + ")|*" + jpgExt + ";*" +jpegExt;
const std::string importWc = "Image files (*" +
    pngExt + ";*" +
    jpgExt + ";*" +
    jpegExt + ")|*" +
        pngExt + ";*" +
        jpgExt + ";*" +
        jpegExt;

ict::Extension extension(const std::string &filename);

#endif // FILEXT_H
