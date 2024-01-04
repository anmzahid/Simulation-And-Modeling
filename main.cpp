
#include <iostream>
#include "Counter.h"
#include "Scheduler.h"
#include "time.h"

int main() {
    srand(33);

    Scheduler *sch = new Scheduler();
    Counter *c = new Counter(0);
    for (int i=0; i<10; i++)
    {
        sch->initialize();
        CarWorkshop::createTraceFile();

        c->initialize();

        /// set input parameters
        c->setMinLevel(1);
        c->setEvaluationInterval(1.0);

        sch->run();


        c->report();

    }

    return 0;
}
