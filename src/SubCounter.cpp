

#include <iostream>
#include <fstream>
#include "SubCounter.h"
#include "Scheduler.h"
#include "CarWorkshop.h"
#include "Counter.h"


SubCounter::SubCounter(int id, int lvl, double prob, double unit, double minA, double maxA, int n, Counter *c, vector<SubCounter*>* nxt) : id_(id), level_(lvl), probability_(prob), counter_(c), next_(nxt), consumableUnit_(unit), minAmount_(minA), maxAmount_(maxA) {
    // initialize stations vector
    this->servers = vector<CarWorkshop*>(n);
    //int id, double minLevel, double maxLevel, double departureMean, double evaluationInterval, double refillMinLag, double refillMaxLag, SubCounter* sc
    for (int i = 0; i < servers.size(); ++i) {
        servers[i] = new CarWorkshop(i, maxAmount_, 500.0, 0.25, 1.0, 5.0, 7.0, this);


    }
}

void SubCounter::initialize() {
    /// call initialize on servers
    for (int i = 0; i < servers.size(); ++i) {
        servers[i] -> initialize();
    }
}

void SubCounter::arrivalHandler(Car *cus) {
    /// choose server and admit customer

    // find idle servers or server with the shortest queue
    CarWorkshop* server = getShortestQueueServer();
    cus -> serverArrivalTime() = Scheduler :: now();
    server ->arrivalHandler(cus);
}

CarWorkshop *SubCounter::getShortestQueueServer() {
    // get pointer to the server with the shortest number of customers
    // in case of tie, return leftmost one
    int sid = 0;
    for (int i = 0; i < servers.size(); ++i) {
        if (servers[i] -> status() + servers[i] -> queueLength()
            < servers[sid] -> status() + servers[sid] -> queueLength()) {
            sid = i;
        }
    }
    return servers[sid];
}

string SubCounter::getAddress() {
    return  to_string(level_) + "-" + to_string(id_);
}



void SubCounter::departureHandler(Car *car) {
    if (car -> path().empty()) {
        // customer has finished service
        return;
    }
    pair<SubCounter*, pair<double, double> > psas = car -> path().front();
    car -> path().pop();
    SubCounter* nextSubCounter = psas.first;
    double amount = psas.second.first;
    double serviceTime = psas.second.second;
    car -> consumableAmount() = amount;
    car -> serviceTime() = serviceTime;
    nextSubCounter -> arrivalHandler(car);

}
double SubCounter::getCarAmount() {
    double u = (double)rand() / RAND_MAX;
    double l = minAmount_ / consumableUnit_;
    double h = maxAmount_ / consumableUnit_;
    return consumableUnit_* floor(l + (h - l + 1) * u );
}

void SubCounter::terminationHandler() {
    for (auto s : servers) {
        s -> terminationHandler();
    }
}

void SubCounter::report() {
    cout << "Sub-counter: " << id() << endl;
    for (auto s : servers) {
        s -> report();
    }

}

double SubCounter::getCarServiceTime() {
    CarWorkshop* server = servers[0];
    return server -> getCustomerServiceTime();
}

void SubCounter::setMinLevel(bool l) {
    for (auto s : servers) {
        if (l == 0) s -> minLevel() = maxAmount_;
        else s -> minLevel() = 2 * maxAmount_;
    }
}

void SubCounter::setEvaluationInterval(double i) {
    for (auto s : servers)
        s -> evaluationInterval() = i;
}

void SubCounter::finalReport() {
    cout << "Sub-counter: " << id() << endl;
    for (auto s : servers) {
        s -> finalReport();
    }
}
