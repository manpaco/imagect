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

Scaler::Scaler(double xf, double yf, ict::ScaleType st) {
    setNewFactor(xf, yf);
    setScaleType(st);
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

void Scaler::setScaleType(ict::ScaleType st) {
    this->st = st;
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

void Scaler::addFactor(double axf, double ayf) {
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

int Scaler::scale(const int v, const ict::Dot d, const double f) const {
    double dValue;
    if (d == ict::IN_D) dValue = v * f;
    else dValue = v / f;
    if (st == ict::FLOOR_ST) return floor(dValue);
    else if (st == ict::ROUND_ST) return round(dValue);
    else return ceill(dValue);
}

int Scaler::scaleX(const int v, ict::Dot d) const {
    double xf;
    getNewFactor(&xf, nullptr);
    return scale(v, d, xf);
}

int Scaler::scaleY(const int v, ict::Dot d) const {
    double yf;
    getNewFactor(nullptr, &yf);
    return scale(v, d, yf);
}

wxPoint Scaler::scalePoint(const wxPoint &p, ict::Dot d) const {
    return wxPoint(scaleX(p.x, d), scaleY(p.y, d));
}

wxSize Scaler::scaleSize(const wxSize &s, ict::Dot d) const {
    return wxSize(scaleX(s.GetWidth(), d), scaleY(s.GetHeight(), d));
}

wxRect Scaler::scaleRect(const wxRect &r, ict::Dot d) const {
    return wxRect(scalePoint(r.GetPosition(), d), scaleSize(r.GetSize(), d));
}

int Scaler::transferX(const int v, ict::Dot d) const {
    double xtf;
    getTransferFactor(&xtf, nullptr);
    return scale(v, d, xtf);
}

int Scaler::transferY(const int v, ict::Dot d) const {
    double ytf;
    getTransferFactor(nullptr, &ytf);
    return scale(v, d, ytf);
}

wxPoint Scaler::transferPoint(const wxPoint &p, ict::Dot d) const {
    return wxPoint(transferX(p.x, d), transferY(p.y, d));
}

wxSize Scaler::transferSize(const wxSize &s, ict::Dot d) const {
    return wxSize(transferX(s.GetWidth(), d), transferY(s.GetHeight(), d));
}

wxRect Scaler::transferRect(const wxRect &r, ict::Dot d) const {
    return wxRect(transferPoint(r.GetPosition(), d), transferSize(r.GetSize(), d));
}

Scaler::~Scaler() {

}
