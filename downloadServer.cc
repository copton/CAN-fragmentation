/** \file downloadServer.cc
 *
 * Implementation of a server in download mode.
 * 
 * This class is part of the fragmentation layer. It provides an implementation 
 * of a server in download mode. 
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
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#include "debug.h"
#include "downloadServer.h"
#include "responseEvent.h"  
#include "requestChannelEvent.h"   
#include "requestEvent.h"
#include "ackEvent.h"

subject_t DownloadServer::SUBJECT_REQUEST_CHANNEL = 0x77777777;  ///< The event tag of the request channel

/** \brief event handler for cosmic events on the request channel.
 *         Calls the special event handlers
 *
 *  \param arg Pointer to downloadServer object
 *         the current cosmic protocol stack does not support passing parameters to callback functions
 */

void DownloadServer::eventHandler(void *arg)
{
    DEBUGOUT("DownloadServer::eventHandler\n");
// the current cosmic protocol stack does not support passing parameters to callback functions
    DownloadServer* that = instance; // get the one and only server object

    RequestChannelEvent event;

    if (getEvent(&that->m_eventQueue, &event)) {
        if (event.isRequest()) {
            DEBUGOUT("DownloadServer::eventHandler: received request\n");
            RequestEvent currentEvent(event);

            // todo: check for the right producer nodeid
            if (currentEvent.content.producer != 0) {
                DEBUGOUT("DownloadServer::eventHandler: request is not addressed to me\n");
                return;
            } 
            
            that->receiveREQUEST(currentEvent);

        } else {
            DEBUGOUT("DownloadServer::eventHandler: received ACK\n");
            AckEvent currentEvent(event);
            that->receiveACK(currentEvent);
        } 
    } else {
        DEBUGOUT("DownloadServer::eventHandler: failed to getEvent()\n");
    }
}

/** \brief event handler for REQEUST messages. Is called by DownloadServer::eventHandler
 *
 * \param event The received RequestEvent
 */

void DownloadServer::receiveREQUEST(RequestEvent& event)
{
    pthread_mutex_lock(&stateMachine.mutex);
    ResponseEvent response;
    

// TODO: set the right consumer nodeid here
    response.content.consumer = 0;

    if (stateMachine.state == IDLE) {
        if (event.content.select > numberOfDocuments) {
            DEBUGOUT("DownloadServer::receiveRequest: selected document does not exist => DENY\n");
            response.DENY();
        } else {
            DEBUGOUT("DownloadServer::receiveRequest: sending OK\n");
	    stateMachine.state = SERVING;
            pthread_cond_signal(&stateMachine.notify);
            if (transfer != NULL) {
                delete transfer;
            }
            transfer = new ServerTransfer(documents[event.content.select].data, documents[event.content.select].size);
            transfer->setFrameSize(event.content.size);
            transfer->setFrameRate(event.content.rate);
            response.OK();
        }
    } else {
        DEBUGOUT("DownloadServer::receiveRequest: not in IDLE state => DENY\n");
        response.DENY();
    }
    m_dataChannel.publish(&response);
    pthread_mutex_unlock(&stateMachine.mutex);
}

/** \brief event handler for Acknowledge (ACK) messages. Is called by DownloadServer::eventHander
 *
 * \param ack Te received AckEvent
 */

void DownloadServer::receiveACK(AckEvent& ack)
{
    pthread_mutex_lock(&stateMachine.mutex);
    if (stateMachine.state == WAITING) {
        //TODO: check for the right producer id and event tag
        if (ack.content.producer == 0) {
            if (transfer->getCRC() != ack.content.crc) {
                DEBUGOUT("DownloadServer::receiveACK: wrong checksum\n");
                transfer->resetFrame();
            } else {
                DEBUGOUT("DownloadServer::receiveACK: good checksum\n");
                transfer->newFrame();
            }
            pthread_cond_signal(&stateMachine.notify);
        } else {
            DEBUGOUT("DownloadServer::receiveACK: acknowledge is not addressed to me.\n");
        }
    } else {
        DEBUGOUT("DownloadServer::receiveACK: not in WAITING state\n");
    }
    pthread_mutex_unlock(&stateMachine.mutex);
}

/** \brief sends the end of frame messages through the data channel
 */

void DownloadServer::sendEndOfFrame()
{
    DEBUGOUT("DownloadServer::sendEndOfFrame()\n");
    DataEvent event;
    event.beEndOfFrame();
    m_dataChannel.publish(&event);
}

/** \brief sends the end of transmission messages through the data channel
 */

void DownloadServer::sendEndOfTransmission()
{
    DEBUGOUT("DownloadServer::sendEndOfTransmission()\n");
    DataEvent event;
    event.beEndOfTrans();
    m_dataChannel.publish(&event);
}

/** \brief sends a complete frame through the data channel
 *  uses the serverTransfer object
 */

void DownloadServer::sendFrame()
{
    DEBUGOUT("DownloadServer::sendFrame()\n");
    DataEvent event;
    while(stateMachine.running && transfer->getNextMessage(event)) {
        m_dataChannel.publish(&event);        
        pthread_mutex_unlock(&stateMachine.mutex);
        transfer->delay();
        pthread_mutex_lock(&stateMachine.mutex);
    }
}

/** \brief run method for the server thread
 *         implements the main state machine
 *         blocks on pthread_conditions to get synchronised with the event handlers.
 * \param arg Pointer to the download Server object
 * \return always NULL
 */

void* DownloadServer::run(void* arg)
{
    DownloadServer* that = (DownloadServer*) arg;
    pthread_mutex_lock(&that->stateMachine.mutex);

    that->stateMachine.state = IDLE;
    while(that->stateMachine.running) {
	DEBUGOUT("DownloadServer::run: state = %d\n", that->stateMachine.state);
        switch(that->stateMachine.state) {
        case IDLE:
            pthread_cond_wait(&that->stateMachine.notify, &that->stateMachine.mutex);
	    DEBUGOUT("DownloadServer::run: received condition signal\n");
            break;
        case SERVING:
	    DEBUGOUT("DownloadServer::run: sending frame\n");
            that->sendFrame();
            that->sendEndOfFrame();
            that->stateMachine.state = WAITING;
            break;
        case WAITING:
            // TODO: timeout
            pthread_cond_wait(&that->stateMachine.notify, &that->stateMachine.mutex);
	    DEBUGOUT("DownloadServer::run: received cond signal\n");
            if (that->transfer->hasFinished()) {
                that->sendEndOfTransmission();
                that->stateMachine.state = IDLE;
            } else {
                that->stateMachine.state = SERVING;
            }
            break;
	default: 
	    DEBUGOUT("ERROR: DownloadServer::run: illegal value: stateMachine.state\n"); 		
assert(false);
        }
    }

    pthread_mutex_unlock(&that->stateMachine.mutex);
    return NULL;
}


DownloadServer* DownloadServer::instance = NULL;   ///< The one and only DownloadServer object. Work around for the event handlers as the current cosmic implementation can not pass arguments to the handlers

/** \brief Constructor. Initiates everything but does not start the server thread
 *
 * \param subjectDataChannel the subject of the data channel
 * \param documents Pointer to list of Documents which the server provides for transfer
 * \param numberOfDocuments The size of the document list
 */

DownloadServer::DownloadServer(subject_t subjectDataChannel, Document* documents, int numberOfDocuments) 
    : documents(documents), numberOfDocuments(numberOfDocuments) , transfer(NULL)
{

    /* little hack for cosmic event callback function */
    if (instance != NULL) {
        throw Exception();
    } else {
        instance = this;
    }

    event_queue_init(&m_eventQueue, 10);
    pthread_mutex_init(&stateMachine.mutex, 0);
    pthread_cond_init(&stateMachine.notify, 0);

    attribute_list_t dataChannelAttrLst; 
    dataChannelAttrLst.periodicSporadic = SPORADIC;
    if (m_dataChannel.announce(subjectDataChannel, &dataChannelAttrLst, 0)) {
        throw Exception();
    }

    sleep(1);
    
    filter_attribute_list_t requestChannelAttrLst;
    if (m_requestChannel.subscribe(SUBJECT_REQUEST_CHANNEL, &requestChannelAttrLst, &m_eventQueue, eventHandler)) {
        throw Exception();
    }
}

/** \brief Destructor. Terminates server thread und cancels subscriptions
 */

DownloadServer::~DownloadServer()
{
    pthread_cancel(m_thread);   
    m_dataChannel.cancelPublication();
    m_requestChannel.cancelSubscription();
    if (transfer != NULL) {
        delete transfer;
    }
}

/** \brief starts the server thread and returns 
 */

void DownloadServer::start()
{
    struct sched_param param;
    param.sched_priority = NRT_THREAD_PRIORITY;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    pthread_create(&m_thread, NULL, run, this);
}

/** \brief signals the server thread to terminate and blocks until run() returns
 */

void DownloadServer::join()
{
    pthread_mutex_lock(&stateMachine.mutex);
    stateMachine.running = false;
    pthread_mutex_unlock(&stateMachine.mutex);

    pthread_join(m_thread, NULL);
}

