#ifndef __REQUEST_EVENT_H
#define __REQUEST_EVENT_H

#include "requestChannelEvent.h"

class RequestEvent : public RequestChannelEvent{
public:
    RequestEvent(const RequestChannelEvent& ev);
    RequestEvent();

    struct {
        u_int16_t eventTag;
        u_int8_t size;
        u_int8_t rate;
        u_int8_t select; 
        u_int16_t producer;
    } content;

    int serialize();

private:
    // this event can not be received but is always copied from a RequestChannelEvent
    int dezerialise(cosmic_event_t* tmpEvent);
};


#endif
