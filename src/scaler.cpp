#include "scaler.h"
#include "defs.h"
#include <cmath>
#include <math.h>

Scaler::Scaler() {

}

Scaler::Scaler(double xxf, double yyf, ict::ScaleType st) {
    setScaleFactor(xxf, yyf);
    setScaleType(st);
}

void Scaler::setScaleFactor(double xxf, double yyf) {
    this->xxFactor = xxf;
    this->yyFactor = yyf;
}

void Scaler::setScaleType(ict::ScaleType st) {
    this->st = st;
}

void Scaler::getScaleFactor(double *xxf, double *yyf) const {
    if (xxf) *xxf = this->xxFactor;
    if (yyf) *yyf = this->yyFactor;
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
    return scale(v, d, xxFactor);
}

int Scaler::scaleY(const int v, ict::Dot d) const {
    return scale(v, d, yyFactor);
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

Scaler::~Scaler() {

}
