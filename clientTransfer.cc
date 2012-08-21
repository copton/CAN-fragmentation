/** \file clientTransfer.cc
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

#include "clientTransfer.h"
#include "debug.h"

/** \brief constructor
 * \param buffer Pointer to buffer
 * \param size Size of the buffer
 */
ClientTransfer::ClientTransfer(char* buffer, int size)
    : m_buffer(buffer), m_size(size), m_done(0), m_startOfFrame(0)
{ }


/** \brief return the number of received bytes 
 * \return number of received bytes
 */
int ClientTransfer::getSize()
{
    return m_done;
}

/** \brief return pointer to buffer
 * \return Pointer to buffer
 */
char* ClientTransfer::getBuffer()
{
    DEBUGOUT("get Buffer: ");
    for (int i = 0; i < m_done; i++) {
        DEBUGOUT("%c", m_buffer[i]);
    }
    return m_buffer;
}   

/** \brief calculate CRC sum over the frame's paylod
 * is not implemented yet. Returns always zero
 * \return CRC sum over the curent frame's payload
 */
u_int16_t ClientTransfer::getCRC()
{
    //TODO: calc crc from m_startOfFrame to m_done    
    return 0;
}

/** \brief handle first message of a frame
 *   the first message needs special handling because it carries the control byte
 * \param event The fist DataEvent of the frame
 */
void ClientTransfer::firstMessage(DataEvent& event)
{
    DEBUGOUT("first Message: ");
    if (event.data[0] == DataEvent::REDO_FRAME) {
        m_done = m_startOfFrame;
    } else {
        m_startOfFrame = m_done;
    }

    int i;
    for (i = 0; i < event.size - 1; i++) {
        m_buffer[m_done + i] = event.data[i + 1];
        DEBUGOUT("%c", event.data[i+1]);
    }
    m_done += i;
    DEBUGOUT("\n");
}

/** \brief Handle data message
 *   assembles the messages into the buffer
 *  \param event The DataEvent
 */
void ClientTransfer::nextMessage(DataEvent& event)
{
    int i;
    DEBUGOUT("nextMessage: ");
    for (i = 0; i < event.size; i++) {
        m_buffer[m_done + i] = event.data[i];
	DEBUGOUT("%c", event.data[i]);
    }
    m_done += i;
    DEBUGOUT("\n");
}



