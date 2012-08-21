/** \file exampleEvent.cc
 *
 *  Example of event class.
 *
 *
 *  University of Ulm. CORTEX Project.
 *
 * \author Cristiano Brudna
 * \date 2003
 *
 */


#include "exampleEvent.h"
#include "error_codes.h"


int exampleEvent::serialize(void) 
{
  int ret;

  start();

  ///< this lines must to be changed for a different event class
  ret = put(content.x);
  ret = put(content.y);

  if(ret)
    return -SERIALIZATION_ERROR;

  return 0;
}


/** 
 *  Copy the data from the string into the fields of the content 
 *
 */
int exampleEvent::deserialize(cosmic_event_t *tmpEvent)
{
  int ret;

  // quality attributes
  nonFunctionalContext.deadline = tmpEvent->deadline;
  nonFunctionalContext.expirationTime = tmpEvent->expirationTime;

  setData(&tmpEvent->data[0], (unsigned int)tmpEvent->len);

  // this lines must be changed for a different event class
  get(&content.x);
  ret = get(&content.y);

  if(ret)
    return -DESERIALIZATION_ERROR;

  return 0;
}


void exampleEvent::print(void)
{
  printf("example event: x=%d, y=%d\n", content.x, content.y);
}
