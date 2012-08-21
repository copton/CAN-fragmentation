
#include <stdio.h>
#include "cosmicEvent.h"

/** \brief This is an example of an event class */
class exampleEvent : public cosmicEvent {

 public:

  // not used in this event class
	struct {
	} functionalContext;


	// this fields are the content of the event
	struct {
		int16_t x;
		int16_t y;
	} content;

 public:
	exampleEvent() : cosmicEvent() {}             // call the constructor of the cosmicEvent class
	int serialize(void);                          // this method is invoked by the middleware after a publish()
	int deserialize(cosmic_event_t *);            // this method is invoked by the middleware before an event notification
	void print(void);
};
