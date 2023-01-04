#ifndef CONTROLLER_H
#define CONTROLLER_H

class wxSize;
class wxPoint;

#include "wx/gdicmn.h"
#include "defs.h"

class CropController {
    public:
        CropController();
        CropController(const wxRect &c);
        ~CropController();
        
        bool cropSize(const wxSize &s);
        bool cropRect(const wxRect &r);
        bool constraint(bool);
        void constraint(wxRect &cons);
        bool modify(const wxPoint &target);
        void press(const wxPoint &p);
        void release();
        void fixRatio(bool op);
        void setRatio(float r);

        wxRect rectZone(ict::Zone) const;
        wxRect relativeToCrop(ict::Zone) const;
        wxSize cropSize() const;
        wxPoint cropPosition() const;
        wxRect cropRect() const;
        ict::Zone getLocation(const wxPoint p) const;
        ict::Zone zonePressed() const;
        bool constraint() const;

    private:
        void cropPosition(const wxPoint &p);
        void updateSizes();
        void pushToConstraint();
        void fitInConstraint();
        void accumulateX(int &dxToCalc, int &dyToUse);
        void accumulateY(int &dyToCalc, int &dxToUse);
        void move(int dx, int dy);
        void resize(const wxPoint &t);
        wxPoint relativeToZone(const wxPoint &p, ict::Zone z);
        bool cropRect(const wxRect &r, bool holdRatio, float initAx, float initAy);

        wxRect crop;
        wxRect iz, nz, sz, ez, wz, nez, nwz, sez, swz;
        float ratio;
        bool fix = false;
        float accumX = 0.0, accumY = 0.0;
        wxRect cons;
        bool activeCons = false;
        ict::Zone fixHint, pressZone;

        wxPoint relativePress;

};

#endif // CONTROLLER_H
