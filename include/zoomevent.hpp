#ifndef ZOOMEVENT_H
#define ZOOMEVENT_H

#include "wx/event.h"

/**
 * Implementation of a zoom event
 *
 * Contains information about zoom value.
 */
class ZoomEvent : public wxEvent {
    public:
        ZoomEvent(wxEventType, int, float);
        float getScaleFactor() const;
        virtual wxEvent * Clone() const;

    private:
        float scaleFactor;
};

wxDECLARE_EVENT(EVT_ZOOM_CHANGE, ZoomEvent);

#endif // ZOOMEVENT_H
