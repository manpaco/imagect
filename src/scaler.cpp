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

Scaler::Scaler(double xf, double yf) {
    setNewFactor(xf, yf);
}

void Scaler::setNewFactor(double xf, double yf) {
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
    double xtf, ytf;
    getTransferFactor(&xtf, &ytf);
    return xtf != 1.0 || ytf != 1.0;
}

void Scaler::clearTransfer() {
    double xf, yf;
    getNewFactor(&xf, &yf);
    setNewFactor(xf, yf);
}

void Scaler::plusFactor(double axf, double ayf) {
    double xf, yf;
    getNewFactor(&xf, &yf);
    xf += axf;
    yf += ayf;
    setNewFactor(xf, yf);
}

void Scaler::getNewFactor(double *xf, double *yf) const {
    if (xf) *xf = this->xxFactor;
    if (yf) *yf = this->yyFactor;
}

void Scaler::getOldFactor(double *xof, double *yof) const {
    if (xof) *xof = this->xxOldFactor;
    if (yof) *yof = this->yyOldFactor;
}

void Scaler::getTransferFactor(double *xtf, double *ytf) const {
    if (xtf) *xtf = xxFactor / xxOldFactor;
    if (ytf) *ytf = yyFactor / yyOldFactor;
}

double Scaler::scale(const double v, const ict::Dot d, const double f) const {
    if (d == ict::IN_D) return v * f;
    else return v / f;
}

double Scaler::scaleX(const double v, ict::Dot d) const {
    double xf;
    getNewFactor(&xf, nullptr);
    return scale(v, d, xf);
}

double Scaler::scaleY(const double v, ict::Dot d) const {
    double yf;
    getNewFactor(nullptr, &yf);
    return scale(v, d, yf);
}

wxPoint2DDouble Scaler::scalePoint(const wxPoint2DDouble &p, ict::Dot d) const {
    return wxPoint(scaleX(p.m_x, d), scaleY(p.m_y, d));
}

double Scaler::transferX(const double v, ict::Dot d) const {
    double xtf;
    getTransferFactor(&xtf, nullptr);
    return scale(v, d, xtf);
}

double Scaler::transferY(const double v, ict::Dot d) const {
    double ytf;
    getTransferFactor(nullptr, &ytf);
    return scale(v, d, ytf);
}

wxPoint2DDouble Scaler::transferPoint(const wxPoint2DDouble &p, ict::Dot d) const {
    return wxPoint(transferX(p.m_x, d), transferY(p.m_y, d));
}

Scaler::~Scaler() {

}
