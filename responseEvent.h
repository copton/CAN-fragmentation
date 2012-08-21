#ifndef __RESPONSE_EVENT_H
#define __RESPONSE_EVENT_H

#include "cosmicEvent.h"


class ResponseEvent : public cosmicEvent {
public:
    struct {
        int16_t consumer;
        int8_t response;
    } content;

    void OK();
    void DENY();
    bool isOK();

    int serialize();
    int deserialize(cosmic_event_t *);

};

#endif
