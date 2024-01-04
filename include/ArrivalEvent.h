#ifndef ARRIVALEVENT_H
#define ARRIVALEVENT_H


#include "Event.h"

class Counter;

class ArrivalEvent : public Event {
private:
    Counter* counter;
public:
    ArrivalEvent (Counter* c) : counter(c) { }
    void handle ();
};


#endif
