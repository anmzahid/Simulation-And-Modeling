#include "DepartureEvent.h"
#include "CarWorkshop.h"

DepartureEvent::DepartureEvent(CarWorkshop *s) : server(s) {

}

void DepartureEvent::handle() {
    server -> departureHandler();
}
