#ifndef SCALER_H
#define SCALER_H

#include <wx/gdicmn.h>
#include "defs.h"

class Scaler {
public:
    Scaler();
    Scaler(double xxf, double yyf, ict::ScaleType st);

    void setScaleFactor(double xxf, double yyf);
    void setScaleType(ict::ScaleType st);
    void getScaleFactor(double *xxf, double *yyf) const;

    int scaleX(const int v, ict::Dot d) const;
    int scaleY(const int v, ict::Dot d) const;
    wxPoint scalePoint(const wxPoint &p, ict::Dot d) const;
    wxSize scaleSize(const wxSize &s, ict::Dot d) const;
    wxRect scaleRect(const wxRect &r, ict::Dot d) const;

    ~Scaler();

private:
    int scale(const int v, const ict::Dot d, const double f) const;
    double xxFactor = 1.0, yyFactor = 1.0;
    ict::ScaleType st = ict::FLOOR_ST;
};

#endif // !SCALER_H
