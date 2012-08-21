/** \file serverTransfer.cc
 * 
 * Serverside fragmentation of data.
 * 
 * This class is part of the fragmentation layer. It provides the serverside fragmentation of data
 * It can be used for download and upload mode.
 * Implementation is not complete yet. No CRC calculation and constant frame rate
 * 
 * University of Ulm. CORTEX Project
 *
 * \author Alexander Bernauer
 * \date 2004
 *
 */

#include "serverTransfer.h"

/** \brief Constructor
 *
 *  \param data Pointer to buffer containing the data
 *  \param size Size of the buffer
 */

ServerTransfer::ServerTransfer(char* data, int size)
    : m_data(data), m_size(size), m_done(0), m_startOfFrame(0), m_messageCount(0), m_redo(false), m_newFrame(false)
{ }

/** \brief set the frame size
 *  \param frameSize The new frame size
 */

void ServerTransfer::setFrameSize(uint8_t frameSize)
{
    m_frameSize = frameSize;
}

/** \brief set the frame rate
 *  \param frameRate The new frame rate
 */

void ServerTransfer::setFrameRate(uint8_t frameRate)
{
    m_frameRate = frameRate;
}

/** \brief open new Frame
 * \param redo True if the current frame has to processed again
 */
void ServerTransfer::newFrame(bool redo) 
{
    m_newFrame = true;
    m_redo = redo;
    m_messageCount = 0;
    m_startOfFrame = m_done;
}

/** \brief resets the current Frame
 *   calls ServerTransfer::newFrame(true)
 */
void ServerTransfer::resetFrame()
{
    newFrame(true);
}

/** \brief check if the whole data has been processed
 *  \return true if the whole data has been processed
 */

bool ServerTransfer::hasFinished()
{
    return (m_done >= m_size);
}

/** \brief calculate the 15-bi CRC sum over the last frame's payload
 *  is not implemented yet. Returns always 0.
 *  \return the 15-bit CRC sum over the last frame's payload
 */
uint16_t ServerTransfer::getCRC()
{
    // TODO:
    // calc crc from m_startOfFrame to m_done    
    return 0;
}

/** \brief suspends execution to meet the frame rate
 *  implementation not complete. Sleeps always 50 milliseconds
 */
void ServerTransfer::delay()
{
    //TODO: sleep appropriate to the transmission rate
    timespec req;
    req.tv_sec = 0;
    req.tv_nsec = 50000000;
    nanosleep(&req, NULL);
}

/** \brief receive new DataEvent containing the next data bytes
 *
 * \param event Reference to the new data event
 * \return false if the message count reiches the frame size
 */

bool ServerTransfer::getNextMessage(DataEvent& event)
{
    if (m_messageCount >= m_frameSize) {
        return false;
    }

    int start = 0;
    
    if (m_newFrame) {
        if (m_redo) {
            event.data[0] = DataEvent::REDO_FRAME;
        } else {
            event.data[0] = 0;
        }
        start = 1;
    }

    int offset;
    int i;
    for (i = start; i < 8; i++) {
        offset = m_done + i;
        if (offset > m_size) break;
        event.data[i] = m_data[offset];
    }
    event.size = i;

    m_done += i;
    m_messageCount++;

    return true;
}
