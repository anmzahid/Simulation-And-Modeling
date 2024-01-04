#ifndef COUNTER_H
#define COUNTER_H

#include "ArrivalEvent.h"
#include "TerminationEvent.h"
#include "SubCounter.h"

class Counter{
public:
    static int carArrived_;

    Counter (int id);
    void initialize();
    void arrivalHandler();
    void terminationHandler();

    void report();
    void finalReport();

    inline int& carArrived () { return (carArrived_); }
    inline double& arrivalMean () { return (arrivalMean_); }
    inline int& id() { return id_; }

    void setMinLevel(bool l);
    void setEvaluationInterval(double i);

private:
    ArrivalEvent a_;
    TerminationEvent t_;
    int id_;
    vector <vector <SubCounter*> > subCounters; // 2D vector of sub-counters (represents levels and choice items)

    // state variables
    bool allowArrival_;

    // input variables
    double arrivalMean_;

    // probability distribution functions
    double exponential ();
    static int discreteRandom ();

};


#endif
