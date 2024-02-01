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

#include "scaler.hpp"
#include "defs.hpp"
#include <cmath>
#include <math.h>
#include <iostream>

Scaler::Scaler() {

}

Scaler::Scaler(wxDouble xf, wxDouble yf) {
    setNewFactor(xf, yf);
}

void Scaler::setNewFactor(wxDouble xf, wxDouble yf) {
    if (xf > 0.0) {
        this->xxOldFactor = this->xxFactor;
        this->xxFactor = xf;
    }
    if (yf > 0.0) {
        this->yyOldFactor = this->yyFactor;
        this->yyFactor = yf;
    }
}

bool Scaler::hasTransfer() const {
    wxDouble xtf, ytf;
    getTransferFactor(&xtf, &ytf);
    return xtf != 1.0 || ytf != 1.0;
}

void Scaler::clearTransfer() {
    wxDouble xf, yf;
    getNewFactor(&xf, &yf);
    setNewFactor(xf, yf);
}

void Scaler::plusFactor(wxDouble axf, wxDouble ayf) {
    wxDouble xf, yf;
    getNewFactor(&xf, &yf);
    xf += axf;
    yf += ayf;
    setNewFactor(xf, yf);
}

void Scaler::getNewFactor(wxDouble *xf, wxDouble *yf) const {
    if (xf) *xf = this->xxFactor;
    if (yf) *yf = this->yyFactor;
}

void Scaler::getOldFactor(wxDouble *xof, wxDouble *yof) const {
    if (xof) *xof = this->xxOldFactor;
    if (yof) *yof = this->yyOldFactor;
}

void Scaler::getTransferFactor(wxDouble *xtf, wxDouble *ytf) const {
    if (xtf) *xtf = xxFactor / xxOldFactor;
    if (ytf) *ytf = yyFactor / yyOldFactor;
}

wxDouble Scaler::scale(const wxDouble v, const ict::Dot d, const wxDouble f) const {
    if (d == ict::IN_D) return v * f;
    else return v / f;
}

wxDouble Scaler::scaleX(const wxDouble v, ict::Dot d) const {
    wxDouble xf;
    getNewFactor(&xf, nullptr);
    return scale(v, d, xf);
}

wxDouble Scaler::scaleY(const wxDouble v, ict::Dot d) const {
    wxDouble yf;
    getNewFactor(nullptr, &yf);
    return scale(v, d, yf);
}

wxPoint2DDouble Scaler::scalePoint(const wxPoint2DDouble &p, ict::Dot d) const {
    return wxPoint2DDouble(scaleX(p.m_x, d), scaleY(p.m_y, d));
}

wxDouble Scaler::transferX(const wxDouble v, ict::Dot d) const {
    wxDouble xtf;
    getTransferFactor(&xtf, nullptr);
    return scale(v, d, xtf);
}

wxDouble Scaler::transferY(const wxDouble v, ict::Dot d) const {
    wxDouble ytf;
    getTransferFactor(nullptr, &ytf);
    return scale(v, d, ytf);
}

wxPoint2DDouble Scaler::transferPoint(const wxPoint2DDouble &p, ict::Dot d) const {
    return wxPoint2DDouble(transferX(p.m_x, d), transferY(p.m_y, d));
}

Scaler::~Scaler() {

}
