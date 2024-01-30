#ifndef SCALER_H
#define SCALER_H

#include <wx/gdicmn.h>
#include "defs.hpp"
#include <wx/geometry.h>

const double minScaleFactor = 1.0 / 256;
const double maxScaleFactor = 256;

class Scaler {
public:
    Scaler();
    Scaler(double xxf, double yyf, ict::ScaleType st);

    void setNewFactor(double xf, double yf);
    void getNewFactor(double *xf, double *yf) const;
    void getOldFactor(double *xof, double *yof) const;
    void getTransferFactor(double *xtf, double *ytf) const;
    bool hasTransfer() const;
    void clearTransfer();
    void addFactor(double axf, double ayf);

    double scaleX(const double v, ict::Dot d) const;
    double scaleY(const double v, ict::Dot d) const;
    wxPoint2DDouble scalePoint(const wxPoint2DDouble &p, ict::Dot d) const;
    wxRect2DDouble scaleRect(const wxRect2DDouble &r, ict::Dot d) const;

    double transferX(const double v, ict::Dot d) const;
    double transferY(const double v, ict::Dot d) const;
    wxPoint2DDouble transferPoint(const wxPoint2DDouble &p, ict::Dot d) const;
    wxRect2DDouble transferRect(const wxRect2DDouble &r, ict::Dot d) const;

    ~Scaler();

private:
    double scale(const double v, const ict::Dot d, const double f) const;

    double xxFactor = 1.0, yyFactor = 1.0;
    double xxOldFactor = 1.0, yyOldFactor = 1.0;
};

#endif // !SCALER_H
