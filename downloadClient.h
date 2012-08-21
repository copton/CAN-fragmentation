/** \file downloadClient.h
 *
 * Implementation of a client in download mode.
 * 
 * This class is part of the fragmentation layer. It provides an implementation 
 * of a client in download mode. 
 * This implementation is not complete yet. It assumes that there is only one 
 * server and its node id is  konw a-priori. No discovery is done.
 *
 * University of Ulm. CORTEX Project
 * 
 * \author Alexander Bernauer
 * \date 2004
 *
 */

#ifndef __DOWNLOAD_CLIENT_H
#define __DOWNLOAD_CLIENT_H

#include "clientTransfer.h"
#include "cosmicEvent.h"
#include <pthread.h>
#include <sentient_object.h>

/** \brief  Implementation of a client in download mode */
class DownloadClient {
public:
    DownloadClient(subject_t dataChannel);
    ~DownloadClient();

    typedef void (*callback_t)(char*, int);     ///< type for callback function
    void start(u_int8_t select, char* buffer, int size, callback_t callback);

    /** \brief generci Exception lcass */
    // TODO: define detailed class hierarchie of exceptions
    class Exception { };

private:
    static void* run(void* arg);

    static subject_t SUBJECT_REQUEST_CHANNEL;   ///< subject of the request channel
    subject_t m_etagDataChannel;                ///< subject of the data channel
    nrtec m_dataChannel;                        ///< the data channel 
    nrtec m_requestChannel;                     ///< the request channel
    event_queue_t m_eventQueue;                 ///< the event queue

    static DownloadClient* instance;            ///< the one and only download client object. Workaround for event handler as current cosmic implementation can not pass arguments to handler
 
    static void eventHandler(void* arg);
    void sendRequest(u_int8_t size, u_int8_t rate, u_int8_t select);
    void sendACK();

    pthread_t m_thread;                         ///< posix thread. running the client's download procedure

    callback_t m_callback;                      ///< callback function to application. Is called when download has finished
    u_int8_t m_select;                          ///< selects one of the server's documents for download

    /** \brief enumarion of the client's states */
    enum State {
        PENDING,
        RECEIVE,
        WAITING,
        FINISHED
    };

    /** \brief 
     * the run method and the event handler communicate using this struct
     */
    struct StateMachine {
        State state;            ///< the current state
        pthread_cond_t finish;  ///< synchronisation between client thread and event handler
        pthread_mutex_t mutex;  ///< mutex for excluseive access to this struct
    } stateMachine;
    
    ClientTransfer* transfer;   ///< the client transfer object
};



#endif
