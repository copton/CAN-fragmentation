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

#include <sentient_object.h>   // standard include for applications
#include <unistd.h>
#include <stdio.h>

#include "debug.h"
#include "downloadClient.h"
#include "responseEvent.h"  // event class used by the application
#include "requestChannelEvent.h"      // event class used by the application
#include "requestEvent.h"
#include "ackEvent.h"
#include <error_codes.h>
//#include "exampleEvent.h"

subject_t DownloadClient::SUBJECT_REQUEST_CHANNEL = 0x10000000; ///< The event tag of the request channel

/** \brief event handler for cosmic events on the data channel
 *
 * \param arg Pointer to downloadClient
 *         the current cosmic protocol stack does not support passing parameters to callback functions
 */
void DownloadClient::eventHandler(void *arg)
{
    DownloadClient* that = instance;
// the current cosmic protocol stack does not support passing parameters to callback functions
    pthread_mutex_lock(&that->stateMachine.mutex); // get the one and only client object

    DataEvent dataEvent;
    ResponseEvent responseEvent;

    DEBUGOUT("DownloadClient::eventHandler: state  = %d\n", that->stateMachine.state);
    
    switch (that->stateMachine.state) {
    case PENDING:
        if(getEvent(&that->m_eventQueue, &responseEvent)) {
            if (responseEvent.isOK()) {
                DEBUGOUT("DownloadClient::eventHandler: received positive response\n");
                that->stateMachine.state = WAITING;
            } else {
                DEBUGOUT("DownloadClient::eventHandler: received negative response\n");
                that->stateMachine.state = FINISHED;
                pthread_cond_signal(&that->stateMachine.finish);
            }
        } else {
            DEBUGOUT("DownloadClient::eventHandler: failed to getEvent()\n");
        }
        break;
    case RECEIVE:
        if(getEvent(&that->m_eventQueue, &dataEvent)) {
            if (dataEvent.isEndOfFrame()) { 
                DEBUGOUT("DownloadClient::eventHandler: received end of frame\n");
                that->sendACK();
                that->stateMachine.state = WAITING; 
            } else {
                DEBUGOUT("DownloadClient::eventHandler: received data message\n");
                that->transfer->nextMessage(dataEvent);        
            }
        } else {
            DEBUGOUT("DownloadClient::eventHandler: failed to getEvent()\n");
        }
        break;
    case WAITING:
        if (getEvent(&that->m_eventQueue, &dataEvent)) {
            if (dataEvent.isEndOfTrans()) {
                DEBUGOUT("DownloadClient::eventHandler: received end of transmission\n");
                that->stateMachine.state = FINISHED;
                pthread_cond_signal(&that->stateMachine.finish);
            } else {
                DEBUGOUT("DownloadClient::eventHandler: received first data message\n");
                that->transfer->firstMessage(dataEvent);
                that->stateMachine.state = RECEIVE;
            }
        } else {
            DEBUGOUT("DownloadClient::eventHandler: failed to getEvent()\n");
        }
    case FINISHED: break;
    }
        
    pthread_mutex_unlock(&that->stateMachine.mutex);
}

/** \brief sends a request for transmission over the request channel
 * \param frameSize The maximum frame size
 * \param frameRate The maximum frame rate
 * \param select Selects one of the server's documents
 */
void DownloadClient::sendRequest(u_int8_t frameSize, u_int8_t frameRate, u_int8_t select)
{
    DEBUGOUT("DownloadClient::sendRequest: ");
    DEBUGOUT("frameSize %d, frameRate %d, select %d\n", frameSize, frameRate, select); 

    RequestEvent requestEvent;
    requestEvent.content.size = frameSize;
    requestEvent.content.rate = frameRate;
    requestEvent.content.select = select;
    //TODO: set the proper producer id
    requestEvent.content.producer = 0;
    int ret;
    if ((ret = m_requestChannel.publish(&requestEvent)) < 0) {
        printError("Error on publish()", ret);
    }
}

/** \brief sends an Acknowledge over the request channel */
void DownloadClient::sendACK()
{
    DEBUGOUT("DownloadClient::sendACK\n");
    //TODO: pass the event tag of the data channel
    AckEvent ackEvent(0xaa);
    ackEvent.content.crc = transfer->getCRC();
    //TODO: set propoer producer id
    ackEvent.content.producer = 0;
    int ret;
    if ((ret = m_requestChannel.publish(&ackEvent)) < 0 ) {
        printError("Error on publish()", ret);
    }
}

/** \brief run method for client thread
 * \param arg Pointer to the download client object
 * \return always NULL
 */
void* DownloadClient::run(void* arg)
{
    DEBUGOUT("DownloadClient::run\n");
    DownloadClient* that = (DownloadClient*) arg;
    pthread_mutex_lock(&that->stateMachine.mutex);
    
    that->sendRequest(3, 0, that->m_select); 
    that->stateMachine.state = PENDING;

    pthread_cond_wait(&that->stateMachine.finish, &that->stateMachine.mutex);
    DEBUGOUT("DownloadClient::run: received condition signal\n");
    
    (*(that->m_callback))(that->transfer->getBuffer(), that->transfer->getSize());

    pthread_mutex_unlock(&that->stateMachine.mutex);
    return NULL;
}

DownloadClient* DownloadClient::instance = NULL;

/** \brief Constructor.
 *  \param etagDataChannel The subject of the data channel
 *  Initialises everything but does not start the client thread
 */
DownloadClient::DownloadClient(subject_t dataChannel) 
    : m_etagDataChannel(dataChannel), transfer(NULL)
{

    /* little hack, because you can not pass any arguments to cosmic event handler functions*/
    if (instance != NULL) {
        throw Exception();
    } else {
        instance = this;
    }

    event_queue_init(&m_eventQueue, 10);
    pthread_mutex_init(&stateMachine.mutex, 0);
    pthread_cond_init(&stateMachine.finish, 0);

    attribute_list_t dataChannelAttrLst; 
    dataChannelAttrLst.periodicSporadic = SPORADIC;
    if (m_requestChannel.announce(SUBJECT_REQUEST_CHANNEL, &dataChannelAttrLst, 0) < 0) {
	DEBUGOUT("anouncment failed\n");
        throw Exception();
    }

    sleep(1);
    
    filter_attribute_list_t requestChannelAttrLst;
    if (m_dataChannel.subscribe(m_etagDataChannel, &requestChannelAttrLst, &m_eventQueue, &eventHandler)) {
        throw Exception();
    }
    sleep(1);
}

/** \brief Destructor
 * Cancels the clients thread. Unsubscribes from channel
 */
DownloadClient::~DownloadClient()
{
    pthread_cancel(m_thread);   
    m_dataChannel.cancelSubscription();
    m_requestChannel.cancelPublication();
    if (transfer != NULL) {
        delete transfer;
    }
}

/** \brief start the client thread
 * \param select Selects one of the server's documents
 * \param buffer Pointer to buffer where the client can copy the data
 * \param size Size of the buffer. Ensure the buffer is large enough.
 * \param callback Callback function void callback(char*, int). Is called when the download has finished with pointer to buffer and number of received bytes.
 */
void DownloadClient::start(u_int8_t select, char* buffer, int size, callback_t callback)
{
    struct sched_param param;
    param.sched_priority = NRT_THREAD_PRIORITY;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    m_callback = callback;
    m_select = select;
    if (transfer != NULL) {
        delete transfer;
    }
    transfer = new ClientTransfer(buffer, size);

    pthread_create(&m_thread, NULL, &run, this);
}


