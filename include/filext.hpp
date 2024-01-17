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

#ifndef FILEXT_H
#define FILEXT_H

#include <string>
#include "defs.hpp"

const std::string pngExt = ".png";
const std::string jpgExt = ".jpg";
const std::string jpegExt = ".jpeg";
const std::string pngWc = "PNG image (*" + pngExt + ")|*" + pngExt;
const std::string jpegWc = "JPEG image (*" + jpgExt +  ";*" + jpegExt + ")|*" +
                           jpgExt + ";*" +jpegExt;
const std::string importWc = "Image files (*" +
    pngExt + ";*" +
    jpgExt + ";*" +
    jpegExt + ")|*" +
        pngExt + ";*" +
        jpgExt + ";*" +
        jpegExt;

/**
 * Return the extension present in the file name.
 *
 * @return file extension. If not idenified returns ict::NONE_EXT
 */
ict::Extension extension(const std::string &filename);

#endif // FILEXT_H
