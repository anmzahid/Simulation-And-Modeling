

#include "Counter.h"
#include "Car.h"
#include "CarWorkshop.h"
#include <cstdlib>
#include <cmath>

int Counter :: carArrived_ = 0;

Counter::Counter(int id) : id_(id), a_(this), t_(this){


    /// instantiate sub-counters (bottom-up)

    //int id, int lvl, double prob, double unit, double minA, double maxA, int n, Counter *c, vector<SubCounter*>* nxt
    SubCounter* carWash1 = new SubCounter(0, 4, 0.45, 2.24, 4, 8, 1, this,nullptr);
    SubCounter* carWash2 = new SubCounter(1, 4, 0.4, 2.05, 4, 8, 1, this, nullptr);

    vector<SubCounter*> facility4 = { carWash1,carWash2 };

    // level 3
    SubCounter* carPolish = new SubCounter(0, 3, 0.6, 0.4, 0, 1, 1, this, &facility4);
    vector<SubCounter*> facility3 = { carPolish };

    // level 2
    SubCounter* part_replacement1 = new SubCounter(0, 2, 0.33, 2.1, 4, 5, 1, this, &facility3);
    SubCounter* part_replacement2 = new SubCounter(1, 2, 0.33, 2.2, 4, 6, 1, this, &facility3);
    SubCounter* part_replacement3 = new SubCounter(2, 2, 0.34, 2.0, 4, 6, 1, this, &facility3);
    vector<SubCounter*> facility2 = { part_replacement1, part_replacement2, part_replacement3 };

    // level 1
    SubCounter* car_repair = new SubCounter(0, 1, 0.9, 1, 2, 3, 1, this, &facility2);
    vector<SubCounter*> facility1 = { car_repair };

    // level 0
    SubCounter* routine_maintenance1 = new SubCounter(0, 0, 0.3, 1.21, 4, 8, 1, this, &facility1);
    SubCounter* routine_maintenance2 = new SubCounter(1, 0, 0.3, 1.32, 4, 8, 1, this, &facility1);
    SubCounter* routine_maintenance3 = new SubCounter(2, 0, 0.4, 1.23, 4, 8, 1, this, &facility1);
    vector<SubCounter*> facility0 = { routine_maintenance1, routine_maintenance2 , routine_maintenance3};


    // level 4

    /// initialize sub-counters and probabilities
    subCounters = vector<vector<SubCounter*> >({facility0, facility1, facility2, facility3, facility4 });
}

double Counter :: exponential() {

    double r = (double)rand()/(RAND_MAX+1);
    double ex = -arrivalMean_ * log (r);
    return ex;

}

void Counter :: initialize () {
    /// initialize the state variables
    allowArrival_ = true;
    arrivalMean_ = 0.5;

    carArrived_ = 0;

    for (auto vs : subCounters) {
        for (auto sc : vs) {
            sc -> initialize();
        }
    }

    /// trigger first arrival event
    double t = CarWorkshop :: exponential(arrivalMean_);
    /// trace file output
    CarWorkshop :: trace_ << "\tinterarrival time = " << t << endl;
    a_.activate(t);

    /// trigger termination event
    t_.activate(12.0); //12 hrs
}

void Counter::arrivalHandler() {
    // check if termination event has occurred - no more arrivals
    if (!allowArrival_) {
        return;
    }
    /// customers arrival in batches

    // determine batch size (Discrete Random)
    int batchSize = CarWorkshop :: discreteRandom();
    /// trace file output
    CarWorkshop :: trace_ << "\tbatch size = "<<batchSize<<endl;

    for ( int i = 0; i < batchSize; i ++ ) {
        // instantiate new customer
        Car *newCustomer = new Car(carArrived_++);

        /// generate random values for each level and determine food amount
        for (auto lvl : subCounters) {
            double u = (double)rand() / RAND_MAX;
            double s = 0.0;
            for (int j = 0; j < lvl.size(); ++j) {
                if (u < s + lvl[j]->probability()) {
                    double amount = lvl[j] -> getCarAmount();
                    double serviceTime = lvl[j] -> getCarServiceTime();
                    newCustomer -> path().push({lvl[j], {amount, serviceTime}});
                    break;
                }
                s += lvl[j]->probability();
            }
        }

        /// send customer to first sub-counter
        pair<SubCounter*, pair<double, double> > psas = newCustomer -> path().front();
        newCustomer -> path().pop();
        SubCounter* nextSubCounter = psas.first;
        double amount = psas.second.first;
        double serviceTime = psas.second.second;
        newCustomer -> consumableAmount() = amount;
        newCustomer -> serviceTime() = serviceTime;
        nextSubCounter -> arrivalHandler(newCustomer);
    }

    // trigger next arrival event
    double t = CarWorkshop :: exponential(arrivalMean_);
    /// trace file output
    CarWorkshop :: trace_ << "\tinterarrival time = " << t << endl;
    a_.activate(t);
}

int Counter::discreteRandom() {
    // for determining batch size (number of cars arriving at a time)
    double u = (double)rand() / (RAND_MAX+1);
    if(u<0.1667) return 1;
    else if(u<0.5001) return 2;
    else if(u<0.8335) return 3;
    return 4;
}

void Counter::terminationHandler() {
    /// disable any more arrivals - simulation will end automatically

    allowArrival_ = false;
    for (auto vsc : subCounters) {
        for ( auto sc : vsc) {
            sc -> terminationHandler();
        }
    }
}

void Counter::report() {
    for(auto vsc : subCounters) {
        for ( auto sc : vsc) {
            sc -> report();
        }
    }
}

void Counter::setEvaluationInterval(double i) {
    for (auto vsc : subCounters) {
        for ( auto sc : vsc) {
            sc -> setEvaluationInterval(i);
        }
    }
}

void Counter::setMinLevel(bool l) {
    for (auto vsc : subCounters) {
        for ( auto sc : vsc) {
            sc -> setMinLevel(l);
        }
    }
}

void Counter::finalReport() {
    for(auto vsc : subCounters) {
        for ( auto sc : vsc) {
            sc -> finalReport();
        }
    }
}
