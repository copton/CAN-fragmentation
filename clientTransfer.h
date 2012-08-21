/** \file clientTransfer.h
 * 
 * Clientside assembly of the framgented data
 * 
 * This class is part of the fragmentation layer. It provides the clientside reassembly of the fragmented data
 * It can be used for download and upload mode.
 * 
 * University of Ulm. CORTEX Project
 *
 * \author Alexander Bernauer
 * \date 2004
 *
 */

#ifndef __TRANSFER_H
#define __TRANSFER_H

#include "dataEvent.h"

/** \brief implementation of the clientsied reassembly of fragmented data */

class ClientTransfer {
public:
    ClientTransfer(char* data, int size);

    void firstMessage(DataEvent& event);
    void nextMessage(DataEvent& event);

    u_int16_t getCRC();
    int getSize();
    char* getBuffer();
private:
    char* m_buffer;     ///< pointer to buffer
    int m_size;         ///< size of buffer

    int m_done;         ///< position within buffer marking how much data has been processed already
    int m_startOfFrame; ///< position within buffer marking the start of the current frame
    
};


#endif
