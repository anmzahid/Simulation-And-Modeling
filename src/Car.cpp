#include "Car.h"
#include "Scheduler.h"
#include "SubCounter.h"

Car ::Car(int id) : id_(id){
    systemArrivalTime_ = Scheduler :: now();
    next_ = nullptr;
}
