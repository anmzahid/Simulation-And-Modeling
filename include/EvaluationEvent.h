#ifndef EVALUATIONEVENT_H
#define EVALUATIONEVENT_H


#include "Event.h"

class CarWorkshop;

class EvaluationEvent : public Event {
private:
    CarWorkshop* server;
public:
    EvaluationEvent (CarWorkshop* s);
    void handle ();
};


#endif
