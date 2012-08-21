#include "ackEvent.h"

AckEvent::AckEvent(int16_t eventTag)
    : RequestChannelEvent(RequestChannelEvent::ACK)
    { 
		content.eventTag = eventTag;
    }

AckEvent::AckEvent(const RequestChannelEvent& ev)
    : RequestChannelEvent(ev)
{
    get(&content.eventTag);
    get(&content.crc);
    get(&content.producer);
}


int AckEvent::serialize()
{
    int ret;

    start();    

    RequestChannelEvent::serialize();
    
    ret = put(content.eventTag);
    ret = put(content.crc);
    ret = put(content.producer);

    return (ret) ? -SERIALIZATION_ERROR : 0;
}
