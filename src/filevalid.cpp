#include "filevalid.h"
#include "identifiersdef.h"

ict::Extension extension(const std::string &filename) {
    bool ext = false;
    std::string reading = "";
    for(int i = 0; i < filename.size(); i++) {
        if(filename[i] == '.' || ext) {
            reading += filename[i];
            ext = true;
        }
    }
    if(reading == pngExt) return ict::PNG_EXT;
    if(reading == jpgExt) return ict::JPG_EXT;
    if(reading == jpegExt) return ict::JPEG_EXT;
    if(reading == "") return ict::NONE_EXT;
    return ict::INVALID_EXT;
}

