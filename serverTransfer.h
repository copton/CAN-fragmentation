/** \file serverTransfer.h
 * 
 * Serverside fragmentation of data.
 * 
 * This class is part of the fragmentation layer. It provides the serverside fragmentation of data
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

#include <stdint.h>
#include "dataEvent.h"

/** \brief implemenation of the serverside fragmenation of data
 */
class ServerTransfer {
public:
    ServerTransfer(char* data, int size);
    void setFrameSize(uint8_t frameSize);
    void setFrameRate(uint8_t frameRate);
    void newFrame(bool redo=false);
    void resetFrame();
    bool hasFinished();
    bool getNextMessage(DataEvent& event);
    void delay();

    uint16_t getCRC();

private:
    int8_t m_frameSize;     ///< the frame size
    int8_t m_frameRate;     ///< the frame rate
    char* m_data;           ///< pointer to buffer containing the data
    int m_size;             ///< size of the buffer

    int m_done;             ///< position within buffer marking how much data has been processed already
    int m_startOfFrame;     ///< position within buffer marking the start of the current frame
    int m_messageCount;     ///< message counter
    
    bool m_redo;            ///< flag for getNextMessage. Reset the current frame
    bool m_newFrame;        ///< flag for getNextMEssage. Opens new Frame
};


#endif
