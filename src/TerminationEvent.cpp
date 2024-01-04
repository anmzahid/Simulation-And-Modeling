#include "TerminationEvent.h"
#include "Counter.h"

void TerminationEvent::handle() {
    // call termination event handler in counter
    counter -> terminationHandler();
}
