#include "Event.h"
#include "Scheduler.h"

void
Event :: activate (double t) {
    Scheduler &s = Scheduler :: instance ();
       stime_ = Scheduler :: now ();
    rtime_ = stime_ + t;

    s.schedule (this);
}
void
Event :: cancel () {
    Scheduler &s = Scheduler :: instance ();
    s.cancel (this);
}
