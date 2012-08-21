#ifndef __DATA_EVENT_H
#define __DATA_EVENT_H

#include "cosmicEvent.h"

class DataEvent : public cosmicEvent {

public:

    // not used in this event class
    struct {
    } functionalContext;


    // these fields are the content of the event

    enum Flag {
        REDO_FRAME = 0x01,
        END_OF_TRANS = 0x02
    };

    DataEvent();

    bool isEndOfFrame();
    bool isEndOfTrans();
    void beEndOfFrame();
    void beEndOfTrans();


    int deserialize(cosmic_event_t *);
    int serialize(void);

    int8_t data[8];     
    int size;  
};

#endif
