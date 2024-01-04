#include "EvaluationEvent.h"
#include "CarWorkshop.h"

EvaluationEvent::EvaluationEvent(CarWorkshop *s) : server(s) {

}

void EvaluationEvent::handle() {
    server -> evaluationHandler();
}
