#include "dataEvent.h"

DataEvent::DataEvent()
    : cosmicEvent()
{ }


int DataEvent::serialize()
{
    int ret;
    for (int i = 0; i < size; i++) {
        ret = put(data[i]);
    }

    return (ret) ? -SERIALIZATION_ERROR : 0;
}

int DataEvent::deserialize(cosmic_event_t *tmpEvent)
{
  int ret;

  // quality attributes
  nonFunctionalContext.deadline = tmpEvent->deadline;
  nonFunctionalContext.expirationTime = tmpEvent->expirationTime;

  setData(&tmpEvent->data[0], (unsigned int)tmpEvent->len);

  size = tmpEvent->len;
  for (int i = 0; i < size; i++) {
      ret = get(&data[i]);
  }


  return (ret) ? -DESERIALIZATION_ERROR : 0;
}


bool DataEvent::isEndOfFrame()
{
    return (size == 0);
}

bool DataEvent::isEndOfTrans()
{
    return ((size == 1) && (data[0] == END_OF_TRANS));
}
    

void DataEvent::beEndOfFrame() 
{
    size = 0;
}

void DataEvent::beEndOfTrans()
{
    size = 1;
    data[0] = END_OF_TRANS;
}
