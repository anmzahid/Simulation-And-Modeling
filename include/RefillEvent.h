#ifndef REFILLEVENT_H
#define REFILLEVENT_H

#include "Event.h"
class CarWorkshop;

class RefillEvent : public Event {
private:
    CarWorkshop* server;
public:
    RefillEvent (CarWorkshop* s);
    void handle ();
};


#endif
