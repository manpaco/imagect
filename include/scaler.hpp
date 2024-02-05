#ifndef SCALER_H
#define SCALER_H

#include <wx/gdicmn.h>
#include "defs.hpp"
#include <wx/geometry.h>

class Scaler {
public:
    Scaler();
    Scaler(wxDouble xxf, wxDouble yyf);

    void setNewFactor(wxDouble xf, wxDouble yf);
    void getNewFactor(wxDouble *xf, wxDouble *yf) const;
    void getOldFactor(wxDouble *xof, wxDouble *yof) const;
    void getTransferFactor(wxDouble *xtf, wxDouble *ytf) const;
    bool hasTransfer() const;
    void clearTransfer();
    void plusFactor(wxDouble pxf, wxDouble pyf);

    wxDouble scaleX(const wxDouble &v, ict::Dot d) const;
    wxDouble scaleY(const wxDouble &v, ict::Dot d) const;
    wxPoint2DDouble scalePoint(const wxPoint2DDouble &p, ict::Dot d) const;
    void scaleRect(wxRect2DDouble *p, ict::Dot d) const;

    wxDouble transferX(const wxDouble &v, ict::Dot d) const;
    wxDouble transferY(const wxDouble &v, ict::Dot d) const;
    wxPoint2DDouble transferPoint(const wxPoint2DDouble &p, ict::Dot d) const;

    ~Scaler();

private:
    wxDouble scale(const wxDouble v, const ict::Dot d, const wxDouble f) const;

    wxDouble xxFactor = 1.0, yyFactor = 1.0;
    wxDouble xxOldFactor = 1.0, yyOldFactor = 1.0;
};

#endif // !SCALER_H
