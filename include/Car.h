#ifndef CAR_H
#define CAR_H


#include <queue>
#include <utility>
using namespace std;

class SubCounter;

class Car {
private:
    int id_;
    Car* next_;
    double systemArrivalTime_;
    double serverArrivalTime_;
    double consumableAmount_;
    double serviceTime_;
    queue<pair<SubCounter*, pair<double, double> > > path_;

public:
    Car (int id);

    inline queue<pair<SubCounter*, pair<double, double> > >& path() { return path_; }
    inline double& consumableAmount() { return consumableAmount_; }
    inline double& serviceTime() { return serviceTime_; }
    inline int& id() { return id_; }
    inline double& systemArrivalTime() { return systemArrivalTime_; }
    inline double& serverArrivalTime() { return serverArrivalTime_; }
    inline Car* next() { return next_; }
};


#endif
