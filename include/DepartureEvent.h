#ifndef DEPARTUREEVENT_H
#define DEPARTUREEVENT_H

#include "Event.h"

class CarWorkshop;

class DepartureEvent : public Event {
private:
    CarWorkshop* server;
public:
    DepartureEvent (CarWorkshop* s);
    void handle ();
};


#endif
