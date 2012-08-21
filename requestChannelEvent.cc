/** \file requestChannelEvent.cc
 *
 *  Event class for a request event. It is used to request upload and download of fragmented data channels. 
 *
 *
 *  University of Ulm. CORTEX Project.
 *
 * \author Cristiano Brudna
 * \date 2004
 *
 */


#include "requestChannelEvent.h"
#include "error_codes.h"

RequestChannelEvent::RequestChannelEvent()
{ }

RequestChannelEvent::RequestChannelEvent(int8_t type) 
    : type(type)
{ }


int RequestChannelEvent::serialize()
{
    return put(type);
}

/** 
 *  Copy the data from the string into the fields of the content 
 *
 */
int RequestChannelEvent::deserialize(cosmic_event_t *tmpEvent)
{
  int ret;

  // quality attributes
  nonFunctionalContext.deadline = tmpEvent->deadline;
  nonFunctionalContext.expirationTime = tmpEvent->expirationTime;

  setData(&tmpEvent->data[0], (unsigned int)tmpEvent->len);

  // this lines must be changed for a different event class
  ret = get(&type);


  if(ret)
    return -DESERIALIZATION_ERROR;

  return 0;
}

bool RequestChannelEvent::isRequest()
{
    return type == REQUEST;
}

bool RequestChannelEvent::isAck()
{
    return type == ACK;
}

