#ifndef __ACK_EVENT
#define __ACK_EVENT

#include "requestChannelEvent.h"


class AckEvent : public RequestChannelEvent{
public:
    AckEvent(int16_t eventTag);
    AckEvent(const RequestChannelEvent& ev);

    struct {
        int16_t eventTag;
        int16_t crc; 
        int16_t producer;
    } content;

    int serialize();

private:
    // this event can not be received but is always copied from a RequestChannelEvent
    int dezerialise(cosmic_event_t* tmpEvent);
};

#endif
