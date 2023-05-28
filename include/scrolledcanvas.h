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

#ifndef SCROLLEDCANVAS_H
#define SCROLLEDCANVAS_H

#include <wx/window.h>

class ScrolledCanvas : public wxWindow {
public:
    ScrolledCanvas();
    ScrolledCanvas(ScrolledCanvas &&);
    ScrolledCanvas(const ScrolledCanvas &);
    ScrolledCanvas &operator=(ScrolledCanvas &&);
    ScrolledCanvas &operator=(const ScrolledCanvas &);
    ~ScrolledCanvas();

private:
    
};

#endif // !SCROLLEDCANVAS_H
