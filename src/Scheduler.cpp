#include "Scheduler.h"
#include "Event.h"

#include <iostream>
#include <fstream>

Event* Scheduler :: eventList_;
double Scheduler :: clock_;
Scheduler* Scheduler::instance_;

Scheduler :: Scheduler () {
	eventList_ = 0;
}

Scheduler :: ~Scheduler () {

}

double
Scheduler :: now () {
	return clock_;
}


Scheduler&
Scheduler :: instance () {
	return (*instance_);
}

void
Scheduler :: schedule (Event* e) {
	addEvent (e);
}


void
Scheduler :: updateClock (double t) {
    // update clock time
    clock_ = t;
}

void
Scheduler :: initialize () {
    // initialize the simulator
    // Hint: What's the value of clock in the beginning?
    clock_ = 0;

}

void
Scheduler :: addEvent (Event *e) {

	Event *current;
	Event *previous;

	// add in an empty list
	if (!eventList_) {
		eventList_ = e;
		e->next_ = 0;

		return;
	}



	previous = eventList_;
	// Add the event as a first element in a non-empty list
	// Hint: check for event expiry time
	// Don't forget to return after adding the event
	if(e->expire() < eventList_->expire()) {
		e->next_ = eventList_;
		eventList_ = e;
		return;
	}




	current = previous -> next_;
	// Add the event as an intermediate element of the eventlist
	while (current != 0) {
		if (e->expire() < current->expire ()) {
			e->next_ = current;
			previous->next_=e;
			return;
		} else {
			current = current->next_;
			previous = previous->next_;
		}
	}

	// add as the last element
	previous->next_ = e;
	e->next_ = 0;

	return;
}

Event*
Scheduler :: removeEvent () {
	Event* temp;

	temp = eventList_;
	eventList_ = eventList_->next_;

	return temp;
}


void
Scheduler :: trigger () {
	Event* temp;

	temp = removeEvent ();
	temp->handle ();
}


void
Scheduler :: run () {
	Event * temp;
	while(eventList_ != 0) {
		temp = removeEvent();

		updateClock(temp->expire());

		temp->handle();
	}
}

void Scheduler::cancel(Event *e) {

}





