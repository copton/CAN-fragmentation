#include "requestEvent.h"
#include "debug.h"

RequestEvent::RequestEvent(const RequestChannelEvent& ev)
    : RequestChannelEvent(ev)
{
    get(&content.eventTag);
    get(&content.size);
    get(&content.rate);
    get(&content.select);
    get(&content.producer);
}

RequestEvent::RequestEvent()
    : RequestChannelEvent(RequestChannelEvent::REQUEST)
{ }


int RequestEvent::serialize() 
{
    int ret;
    start();

    RequestChannelEvent::serialize();

    ret = put(content.eventTag);
    ret = put(content.size);
    ret = put(content.rate);
    ret = put(content.select);
    ret = put(content.producer);

    return (ret) ? -SERIALIZATION_ERROR : 0;
}

