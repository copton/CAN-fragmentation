#ifndef __REQUEST_CHANNEL_H
#define __REQUEST_CHANNEL_H

#include <stdio.h>
#include "cosmicEvent.h"


class RequestChannelEvent : public cosmicEvent {

 public:

  enum Type {
	ACK = 0,
	REQUEST
  };
  // 1 for a request; 0 for acknowledge
  RequestChannelEvent();
  RequestChannelEvent(int8_t type);

  // not used in this event class
  struct {
  } functionalContext;
  
  
  // these fields are the content of the event
  int8_t    type;     
  
 public:
  int deserialize(cosmic_event_t *);            // this method is invoked by the middleware before an event notification

  bool isRequest();
  bool isAck();

 protected:
  int serialize(void);                          // this method is invoked by the middleware after a publish()
};

#endif
