/**
 *     Copyright (C) 2023 Acuilán Gabriel <acuilangabriel@gmail.com>
 *
 *     This file is part of ImageCT.
 *
 *     ImageCT is free software: you can redistribute it and/or modify it under
 *     the terms of the GNU General Public License as published by the Free
 *     Software Foundation, either version 3 of the License, or (at your
 *     option) any later version.
 * 
 *     ImageCT is distributed in the hope that it will be useful, but WITHOUT
 *     ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *     FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *     more details.
 *
 *     You should have received a copy of the GNU General Public License along
 *     with ImageCT. If not, see <https://www.gnu.org/licenses/>.
 */

#include "filext.h"

ict::Extension extension(const std::string &filename) {
    int dotPos = filename.rfind('.');
    std::string s; 
    if(dotPos != std::string::npos) s = filename.substr(dotPos);
    if(s == pngExt) return ict::PNG_EXT;
    if(s == jpgExt) return ict::JPG_EXT;
    if(s == jpegExt) return ict::JPEG_EXT;
    return ict::NONE_EXT;
}

