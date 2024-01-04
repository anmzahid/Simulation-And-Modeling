#ifndef SUBCOUNTER_H
#define SUBCOUNTER_H

#include "CarWorkshop.h"
#include "Car.h"
#include <vector>
using namespace std;

class Counter;

class SubCounter {
private:
    int id_;
    int level_;
    double probability_;
    double consumableUnit_;
    double minAmount_;
    double maxAmount_;
    vector <CarWorkshop* > servers;
    Counter* counter_;
    vector<SubCounter*>* next_;
public:
    SubCounter (int id, int lvl, double prob, double unit, double minA, double maxA, int n, Counter* c, vector<SubCounter*>* nxt);
    void initialize ();
    void arrivalHandler (Car* cus);
    void departureHandler (Car* cus);
    void terminationHandler ();

    void report();
    void finalReport();

    inline int& id() { return id_; }
    inline int& level() { return level_; }
    inline double& probability() { return probability_; }
    inline double& consumableUnit() { return consumableUnit_; }

    void setMinLevel(bool l);
    void setEvaluationInterval(double i);

    string getAddress();
    double getCarAmount();
    double getCarServiceTime();

private:
    CarWorkshop* getShortestQueueServer();

};

#endif
