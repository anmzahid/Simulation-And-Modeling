#include "RefillEvent.h"
#include "CarWorkshop.h"

RefillEvent::RefillEvent(CarWorkshop *s) : server(s) {

}

void RefillEvent::handle() {
    server->refillHandler();
}
