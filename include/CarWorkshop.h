#ifndef CARWORKSHOP_H
#define CARWORKSHOP_H


#include "Car.h"
#include "DepartureEvent.h"
#include "EvaluationEvent.h"
#include "RefillEvent.h"
#include <cmath>
#include <fstream>

class SubCounter;

class CarWorkshop {
private:

    // state variables
    queue <Car*> queue_;
    int status_;
    double consumableLevel_;

    int id_;
    SubCounter* subCounter_;
    Car* carInService_;

    // constants
    double minLevel_;
    double maxLevel_;
    double departureMean_;
    double evaluationInterval_;
    double refillMinLag_;
    double refillMaxLag_;

    // events
    DepartureEvent d_;
    EvaluationEvent e_;
    RefillEvent r_;

    // statistical variables
    double timeLastEvent_;
    double areaBusy_;
    double areaQueue_;
    double areaConsumableLevel_;
    double totalQueueingDelay_;
    double delay_;
    double totalServerDelay_;
    double serverDelay_;
    int carsArrived_;
    int carsServed_;
    int carsLeft_;
    int carsStalled_;
    int refillCount_;
    int maxQueueLength_;
    vector<double> queueingDelays_, serverDelays_;

    bool allowEvaluation_;

    string getServerAddress();

    void clearQueue();
public:
    CarWorkshop(int id, double minLevel, double maxLevel, double departureMean, double evaluationInterval, double refillMinLag, double refillMaxLag, SubCounter* sc);
    void initialize();

    friend std::ostream &operator<<(std::ostream &os, const CarWorkshop &server);

    // state variables accessors
    inline int& status() { return status_; }
    inline int queueLength() { return queue_.size(); }

    // constant accessors
    inline double& minLevel() { return minLevel_; }
    inline double& maxLevel() { return maxLevel_; }
    inline double& departureMean() { return departureMean_; }
    inline double& evaluationInterval() { return evaluationInterval_; }
    inline double& refillMinLag() { return refillMinLag_; }
    inline double& refillMaxLag() { return refillMaxLag_; }

    void arrivalHandler (Car* cus);
    void terminationHandler ();
    void departureHandler ();
    void evaluationHandler ();
    void refillHandler ();

    void updateStat();
    void report();
    void finalReport();

    // static trace file
    static ofstream trace_;

    // report on individual servers
    ofstream report_;

    // store statistical values
    vector<double> served, stalled, serverUtil, avgQueueLen, avgFoodLevel, avgQueueDelay, avgServerDelay, refillCount;

    static void createTraceFile();
    void createReportFile();

    // helper probability functions
    static double exponential (double mean);
    static int discreteRandom ();
    double continuousRandom ();

    inline double getCustomerServiceTime () { return exponential(departureMean_); }
};


#endif
