#ifndef TERMINATIONEVENT_H
#define TERMINATIONEVENT_H

#include "Event.h"

class Counter;


class TerminationEvent : public Event {
private:
    Counter* counter;
public:
    TerminationEvent (Counter* c) : counter(c) { }
    void handle ();
};


#endif
