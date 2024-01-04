
#include <iostream>
#include "CarWorkshop.h"
#include "Scheduler.h"
#include "SubCounter.h"
#include <sstream>
ofstream CarWorkshop :: trace_;

CarWorkshop::CarWorkshop(int id, double minLevel, double maxLevel, double departureMean, double evaluationInterval, double refillMinLag, double refillMaxLag, SubCounter* sc) : id_(id), subCounter_(sc), d_(this), e_(this), r_(this) {
    minLevel_ = 100;
    maxLevel_ = 200;
    departureMean_ = departureMean;
    evaluationInterval_ = evaluationInterval;
    refillMinLag_ = refillMinLag;
    refillMaxLag_ = refillMaxLag;
    createReportFile();
}

void CarWorkshop::initialize() {
    status_ = 0;
    consumableLevel_ = maxLevel_;

    carsArrived_ = 0;
    carsServed_ = 0;
    carsLeft_ = 0;
    carsStalled_ = 0;
    refillCount_ = 0;
    carInService_ = nullptr;

    timeLastEvent_ = 0.0;
    areaBusy_ = 0.0;
    areaQueue_ = 0.0;
    areaConsumableLevel_ = 0.0;
    totalQueueingDelay_ = 0.0;
    totalServerDelay_ = 0.0;
    maxQueueLength_ = 0;

    allowEvaluation_ = true;

    /// trigger first evaluation event
    e_.activate(evaluationInterval_);
}

void CarWorkshop::departureHandler() {

    /// update statistical variables
    updateStat();

    /// a car has finished service at a server
    status() = 0;
    trace_<<"D\t"<<getServerAddress()<<"\t"<<Scheduler::now()<<"\t"<<carInService_->id()<<"\t"<<status()<<"\t"<<queueLength()<<"\t"<<consumableLevel_<<endl;

    carsServed_++;
    serverDelay_ = Scheduler :: now() - carInService_ -> serverArrivalTime();
    totalServerDelay_ += serverDelay_;
    serverDelays_.push_back(serverDelay_);

    /// send to next sub-counter
    subCounter_ ->departureHandler(carInService_);

    if (queueLength() > 0) {
        carInService_ = queue_.front();

        /// check if consumables is available
        if(carInService_ -> consumableAmount() < consumableLevel_) {
            // if yes, decrease consumables level and trigger departure event from server
            status() = 1;
            queue_.pop();

            /// trace file output : service start
            trace_<<"S\t"<<getServerAddress()<<"\t"<<Scheduler::now()<<"\t"<<carInService_->id()<<"\t"<<status()<<"\t"<<queueLength()<<"\t"<<consumableLevel_<<endl;


            // calculate delay
            delay_ = Scheduler :: now() - carInService_ -> serverArrivalTime();
            totalQueueingDelay_ += delay_;
            queueingDelays_.push_back(delay_);

            consumableLevel_ -= carInService_ -> consumableAmount();
            double t = carInService_ -> serviceTime();
            /// trace file output
            trace_<< "\tservice time = " << t << endl;
//            cout << "service time = " << t << endl;
            d_.activate(t);
        } else {
            /// trace file output
            trace_ << "\tinsufficient consumables level"<<endl;
//            carInService_ = nullptr;
//
//            if (!allowEvaluation_) {
//                /// no more refill event, serve first customer with remaining consumable (If any) and clear queue
//                if (consumableLevel_ > 0) {
//                    status() = 1;
//                    carInService_ = queue_.front();
//                    queue_.pop();
//
//                    // calculate delay
//                    delay_ = Scheduler :: now() - carInService_ -> serverArrivalTime();
//                    totalQueueingDelay_ += delay_;
//                    queueingDelays_.push_back(delay_);
//                    consumableLevel_ = 0.0;
//
//                    double t = carInService_ -> serviceTime();
//                    /// trace file output
//                    trace_<< "\tservice time = " << t << endl;
////                    cout << "service time = " << t << endl;
//                    d_.activate(t);
//                }
                clearQueue();
           // } else {
                carsStalled_++;
            //}
        }

    } else {
        carInService_ = nullptr;
    }
}

void CarWorkshop::evaluationHandler() {

    /// No more evaluation and refill after termination
    if (!allowEvaluation_)
        return;

    /// update statistical variables
    updateStat();

    string cid_ = "X";
    if (carInService_) {
        cid_ = to_string(carInService_ -> id());
    }
    /// trace file output
    trace_<<"E\t"<<getServerAddress()<<"\t"<<Scheduler::now()<<"\t"<<cid_<<"\t"<<status()<<"\t"<<queueLength()<<"\t"<<consumableLevel_<<endl;
    double t = continuousRandom();
    if (consumableLevel_ < minLevel_) {
        /// trace file output
        trace_<<"\tdelivery lag = "<<t<<endl;
//        cout << "delivery lag = " << t << endl;
        r_.activate(t);
    }

    /// trigger next evaluation event
    e_.activate(evaluationInterval_);

}

void CarWorkshop::refillHandler() {

    /// update statistical variables
    updateStat();
    refillCount_ ++;
    consumableLevel_ = maxLevel_;
    string cid_ = "X";

    /// trace file output
    trace_<<"R\t"<<getServerAddress()<<"\t"<<Scheduler::now()<<"\t"<<cid_<<"\t"<<status()<<"\t"<<queueLength()<<"\t"<<consumableLevel_<<endl;

    if (status() == 0 && queueLength() > 0) {
        carInService_ = queue_.front();

        /// check if Consumables is available [Probably Redundant]
        if(carInService_ -> consumableAmount() < consumableLevel_) {
            // if yes, decrease food level and trigger departure event from server
            status() = 1;
            queue_.pop();

            /// trace file output : service start
            trace_<<"S\t"<<getServerAddress()<<"\t"<<Scheduler::now()<<"\t"<<carInService_->id()<<"\t"<<status()<<"\t"<<queueLength()<<"\t"<<consumableLevel_<<endl;

            // calculate delay
            delay_ = Scheduler :: now() - carInService_ -> serverArrivalTime();
            totalQueueingDelay_ += delay_;
            queueingDelays_.push_back(delay_);

            consumableLevel_ -= carInService_ -> consumableAmount();
            double t = carInService_ -> serviceTime();
            /// trace file output
            trace_<< "\tservice time = " << t << endl;
//
            d_.activate(t);
        } else {
            /// trace file output
            trace_ << "\tinsufficient consumables level"<<endl;
            carInService_ = nullptr;

            if (!allowEvaluation_) {
                /// no more refill event, serve first car with remaining consumable(If any) and clear queue
                if (consumableLevel_ > 0) {
                    status() = 1;
                    carInService_ = queue_.front();
                    queue_.pop();

                    // calculate delay
                    delay_ = Scheduler :: now() - carInService_ -> serverArrivalTime();
                    totalQueueingDelay_ += delay_;
                    queueingDelays_.push_back(delay_);
                    consumableLevel_ = 0.0;

                    double t = carInService_ -> serviceTime();
                    /// trace file output
                    trace_<< "\tservice time = " << t << endl;
//
                    d_.activate(t);
                }
                clearQueue();
            } else {
                carsStalled_++;
            }
        }

    }
}

void CarWorkshop::arrivalHandler(Car* car) {
    /// update statistical variables
    updateStat();

    carsArrived_++;
    /// trace file output
    /// <Event> <Station> <Time> <Car ID> <Station Status> <Queue Size> <consumable Level>
    trace_<<"A\t"<<getServerAddress()<<"\t"<<Scheduler::now()<<"\t"<<car->id()<<"\t"<<status()<<"\t"<<queueLength()<<"\t"<<consumableLevel_<<endl;

    if (status() == 0) {  /// Station is idle
        /// check if consumables are available
        if(car -> consumableAmount() < consumableLevel_) {
            // if yes, make server busy, decrease food level and trigger departure event from server
            status() = 1;
            trace_<<"S\t"<<getServerAddress()<<"\t"<<Scheduler::now()<<"\t"<<car->id()<<"\t"<<status()<<"\t"<<queueLength()<<"\t"<<consumableLevel_<<endl;

            carInService_ = car;

            /// calculate delay
            delay_ = Scheduler :: now() - carInService_ -> serverArrivalTime();
            totalQueueingDelay_ += delay_;
            queueingDelays_.push_back(delay_);

            consumableLevel_ -= carInService_ -> consumableAmount();
            double t = carInService_ -> serviceTime();
            /// trace file output
            trace_<< "\tservice time = " << t << endl;
//            cout << "service time = " << t << endl;
            d_.activate(t);
        }
        else {
            ///  trace file output
            trace_<<"\tinsufficient consumables level"<<endl;
            queue_.push(car);
            carInService_ = nullptr;

            if (!allowEvaluation_) {
                /// no more refill event, serve first car with remaining consumables(If any) and clear queue
                if (consumableLevel_ > 0) {
                    status() = 1;
                    carInService_ = queue_.front();
                    queue_.pop();

                    // calculate delay
                    delay_ = Scheduler :: now() - carInService_ -> serverArrivalTime();
                    totalQueueingDelay_ += delay_;
                    queueingDelays_.push_back(delay_);
                    consumableLevel_ = 0.0;

                    double t = carInService_ -> serviceTime();
                    /// trace file output
                    trace_<< "\tservice time = " << t << endl;
//                    cout << "service time = " << t << endl;
                    d_.activate(t);
                }
                clearQueue();
            } else {
                carsStalled_++;
            }
        }

    } else
    {
        trace_<<"\tStation busy"<<endl;
        queue_.push(car);
    }
}

double CarWorkshop::exponential(double mean) {
    double r = (double)rand()/(RAND_MAX);
    double ex = -mean * log (r);
    return ex;
}

std::ostream &operator<<(std::ostream &os, const CarWorkshop &server) {
    os << "id_: " << server.id_ << " minLevel_: " << server.minLevel_ << " maxLevel_: " << server.maxLevel_
       << " departureMean_: " << server.departureMean_ << " evaluationInterval_: " << server.evaluationInterval_
       << " refillMinLag_: " << server.refillMinLag_ << " refillMaxLag_: " << server.refillMaxLag_;

    return os;
}

void CarWorkshop::createTraceFile() {
    trace_.open ("trace.out", ios::out);
    if (!trace_) {
        cout << "cannot open the trace file.\n";
    }
    trace_<< "trace file for the simulation" << endl;
    trace_ << "format of the file" << endl;
    trace_ << "<Event> <Station> <Time> <Customer ID> <Station Status> <Queue Size> <Consumables Level>" << endl << endl;
}

string CarWorkshop::getServerAddress() {
    return   subCounter_->getAddress() ;
}

int CarWorkshop::discreteRandom() {
    // for determining batch size (number of cars arriving at a time)
    double u = (double)rand() / (RAND_MAX);
    if(u<0.1667) return 1;
    else if(u<0.3334) return 2;
    else if(u<0.5001) return 3;
    else if(u<0.6668) return 4;
    else if(u<0.8335) return 5;
    return 6;
}

void CarWorkshop::terminationHandler() {
//    // debug
//    cout << "termination handler " << Scheduler :: now() << endl;

    allowEvaluation_ = false;

    if (status() == 0 && queueLength() > 0) { /// if customers waiting for refill event
        clearQueue();
    }
}




int extractLevelFromAddress(const std::string& address) {
    std::istringstream iss(address);
    std::string levelStr;
    std::getline(iss, levelStr, '-');
    return std::stoi(levelStr);
}





void CarWorkshop::report() {
    /// update statistical variables
    updateStat();




    cout << "Station: " << getServerAddress() << endl;
     string address = getServerAddress();

        // Call the extractLevel function
        int extractedLevel = extractLevelFromAddress(address);
    // cout<<"Testing : "<<extractedLevel<<endl;

//    report_ << getServerAddress() << ",";
    cout << "Total Cars Arrived: " << carsArrived_ << endl;
//    report_ << customersArrived_ << ",";
    cout << "Total Cars Served: " << carsServed_ <<endl;
    report_ << 100.0 * carsServed_ / (double)carsArrived_ << ",";
//    report_served << getServerAddress() << "," << 100.0 * customersServed_ / (double)customersArrived_ << endl;
    served.push_back(100.0 * carsServed_ / (double)carsArrived_);

    cout << "Total Cars Stalled: " << carsStalled_ << endl;
    report_ << 100.0 * carsStalled_ / (double)carsArrived_ << ",";
//    report_stalled << getServerAddress() << "," << 100.0 * customersStalled_ / (double)customersArrived_ << endl;
    stalled.push_back(100.0 * carsStalled_ / (double)carsArrived_);

    cout << "Total Cars Left Unserved: " << carsLeft_ <<endl;
//    report_ << customersLeft_ << ",";

    cout << "Average Station Utilization: " << 100 * areaBusy_ / Scheduler :: now() << "%" << endl;
    report_ << 100 * areaBusy_ / Scheduler :: now() << ",";
//    report_serverUtil << getServerAddress() << "," << 100 * areaBusy_ / Scheduler :: now() << endl;
    serverUtil.push_back(100 * areaBusy_ / Scheduler :: now());

    cout << "Average Queue Length: " << areaQueue_ / Scheduler :: now() << endl;
    report_ << areaQueue_ / Scheduler :: now() << ",";
//    report_queueLength << getServerAddress() << "," << areaQueue_ / Scheduler :: now() << endl;
    avgQueueLen.push_back(areaQueue_ / Scheduler :: now());

    report_ << maxQueueLength_ << ",";

    cout << "Average Consumables Level: " << areaConsumableLevel_ / Scheduler :: now() << endl;
    report_ << areaConsumableLevel_ / Scheduler :: now() << ",";
//    report_foodLevel << getServerAddress() << "," << areaFoodLevel_ / Scheduler :: now() << endl;
    avgFoodLevel.push_back(areaConsumableLevel_ / Scheduler :: now());

    cout << "Average Queue Delay: " << totalQueueingDelay_ / carsArrived_ << endl;
    report_ << totalQueueingDelay_ / carsArrived_ << ",";
//    report_queueDelay << getServerAddress() << "," << totalQueueingDelay_ / customersArrived_ << endl;
    avgQueueDelay.push_back(totalQueueingDelay_ / carsArrived_);

    cout << "Average Service Delay: " << totalServerDelay_ / carsServed_ << endl;
    report_ << totalServerDelay_ / carsServed_ << ",";
//    report_serverDelay << getServerAddress() << "," << totalServerDelay_ / customersServed_ << endl;
    avgServerDelay.push_back(totalServerDelay_ / carsServed_);


    cout << "Number of refill events: " << refillCount_ << endl;
    report_ << refillCount_ << ',';
//    report_refillCount << getServerAddress() << "," << refillCount_ << endl;
    refillCount.push_back(refillCount_);

    cout << "Remaining Consumables: " << consumableLevel_ << endl;
    report_ << consumableLevel_ << endl;




    cout<<endl<<endl<<"/////////";


    // debug
    if (carsArrived_ != carsServed_ + carsLeft_) cout << "XXXXXXXXXX" <<endl;
}

void CarWorkshop::updateStat() {
    double duration_ = Scheduler :: now() - timeLastEvent_;
    areaBusy_ += duration_ * status();
    areaQueue_ += duration_ * queueLength();
    areaConsumableLevel_ += duration_ * consumableLevel_;
    timeLastEvent_ = Scheduler :: now();
    maxQueueLength_ = max(maxQueueLength_, queueLength());
}

void CarWorkshop::clearQueue() {
    while (queueLength() > 0) {
        Car* unservedCar_ = queue_.front();
        queue_.pop();

        carsLeft_ ++;

        trace_<<"N\t"<<getServerAddress()<<"\t"<<Scheduler::now()<<"\t"<<unservedCar_->id()<<"\t"<<status()<<"\t"<<queueLength()<<"\t"<<consumableLevel_<<endl;

        delay_ = Scheduler :: now() - unservedCar_ -> serverArrivalTime();
        totalQueueingDelay_ += delay_;

        /// send to next sub-counter
        subCounter_ ->departureHandler(unservedCar_);
    }
}

double CarWorkshop::continuousRandom() {
    return refillMinLag_ + (refillMaxLag_ - refillMinLag_) * (double)rand()/(RAND_MAX);
}

void CarWorkshop::createReportFile() {
    /// create report file
    report_.open ("report" + getServerAddress() + ".csv", ios::out);
    if (!report_) {
        cout << "cannot open the report file.\n";
        return;
    }

    report_ << "% Served, % Stalled, Avg Station Utilization, Avg Queue Length, Max Queue Length, Avg Consumables Level, Avg Queue Delay, Avg Service Delay, # refill events, food left" << endl;

}


void CarWorkshop::finalReport() {

}
