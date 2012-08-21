/** \file downloadServer.h
 *
 * Implementation of a server in download mode.
 * 
 * This class is part of the fragmentation layer. It provides an implementation 
 * of server in download mode. 
 * This implementation is not complete yet. It assumes that there is only one 
 * server and its node id is  konw a-priori. No discovery is done.
 *
 * University of Ulm. CORTEX Project
 * 
 * \author Alexander Bernauer
 * \date 2004
 *
 */

#ifndef __DOWNLOAD_SERVER_H
#define __DOWNLOAD_SERVER_H

#include "requestEvent.h"
#include "ackEvent.h"
#include "serverTransfer.h"
#include <pthread.h>
#include <sentient_object.h>

/** \brief Implementation of a server in download mode */
class DownloadServer {
public:
    /** \brief struct for documents which the server provides for transfer */
    struct Document {
        char* data;  ///< pointer to buffer containing the document data
        int size;    ///< size of the buffer
    };
    
    DownloadServer(subject_t etagDataChannel, Document* documents, int numberOfDocuments);
    ~DownloadServer();

    void start();
    void join();

    /** \brief generic Exception class. 
     */
    // TODO: define detailed class hierarchie of exceptions
    class Exception {

    };
 
private:
    Document* documents;     ///< pointer to list of documents
    int numberOfDocuments;   ///< size of document list

    static subject_t SUBJECT_REQUEST_CHANNEL;   ///< subject of the request channel
    subject_t m_etagDataChannel;                ///< subject of the data channel
    nrtec m_dataChannel;                        ///< the data channel
    nrtec m_requestChannel;                     ///< the request channel
    event_queue_t m_eventQueue;                 ///< the event queue

    static void* run(void* arg);                
    pthread_t m_thread;                         ///< posix thread. running the server's state machine

    static DownloadServer* instance;            ///< the one and only download server object. Workaround for event handler as current cosmic implementation can not pass arguments to handler
    static void eventHandler(void* arg);
    void receiveREQUEST(RequestEvent& event);
    void receiveACK(AckEvent& event);	
    void sendEndOfFrame();
    void sendFrame();
    void sendEndOfTransmission();

    /** \brief enumeration of the server's states */
    enum State {
        IDLE,
        SERVING,
        WAITING
    };

    /** \brief 
     * the run method and the event handler communicate using this struct
     */
    struct StateMachine {
        State state;                ///< the current state
        pthread_cond_t notify;      ///< synchronisation between server thread and event handler
        pthread_mutex_t mutex;      ///< mutex for exclusive access to this struct
        bool running;               ///< true while server thread shall run
    } stateMachine;
    

    ServerTransfer* transfer;       ///< the server transfer object
};

#endif

