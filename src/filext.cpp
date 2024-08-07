#include "filext.hpp"

ict::Extension extension(const std::string &filename) {
    int dotPos = filename.rfind('.');
    std::string s;
    if(dotPos != std::string::npos) s = filename.substr(dotPos);
    if(s == pngExt) return ict::PNG_EXT;
    if(s == jpgExt) return ict::JPG_EXT;
    if(s == jpegExt) return ict::JPEG_EXT;
    return ict::NONE_EXT;
}

