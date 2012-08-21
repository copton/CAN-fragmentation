#include "responseEvent.h"


void ResponseEvent::OK() 
{
    content.response = 1;
}

void ResponseEvent::DENY()
{
    content.response = 0;
}


bool ResponseEvent::isOK()
{
    return content.response == 1;
}
 

int ResponseEvent::serialize()
{
    int ret;
    start();

    ret = put(content.consumer);
    ret = put(content.response);

    return (ret) ? -SERIALIZATION_ERROR : 0;
}


int ResponseEvent::deserialize(cosmic_event_t* tmpEvent)
{
    setData(&tmpEvent->data[0], (unsigned int) tmpEvent->len);

    int ret;
    ret = get(&content.consumer);
    ret = get(&content.response);
    
    return (ret) ? -DESERIALIZATION_ERROR : 0;
}
