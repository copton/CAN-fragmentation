include ../../cosmic.mk


HEADERS = ../layers/cosmic_types.h \
	../layers/event_queue.h \
	../other/log_msg.h \
	../layers/srtec.h \
	../layers/nrtec.h \
	../layers/sentient_object.h

LIBS = ../../layers/cosmic.o -lpthread

COMM_OBJS = requestChannelEvent.o \
	requestEvent.o \
	responseEvent.o \
	ackEvent.o \
	dataEvent.o \
	exampleEvent.o

CLIENT_OBJS = clientMain.o \
	clientTransfer.o \
	downloadClient.o

SERVER_OBJS = serverMain.o\
	serverTransfer.o \
	downloadServer.o

all: client server

client: $(COMM_OBJS) $(CLIENT_OBJS) 
	$(CXX) ${INCLUDE} ${CXXFLAGS} -o $@ $^ ${LIBS} 

server: $(COMM_OBJS) $(SERVER_OBJS) 
	$(CXX) ${INCLUDE} ${CXXFLAGS} -o $@ $^ ${LIBS} 




serverMain.o: serverMain.cc
	$(CXX) ${INCLUDE} ${CXXFLAGS} -c $< 

clientMain.o: clientMain.cc
	$(CXX) ${INCLUDE} ${CXXFLAGS} -c $< 

downloadServer.o: downloadServer.cc downloadServer.h
	$(CXX) ${INCLUDE} ${CXXFLAGS} -c $< 

downloadClient.o: downloadClient.cc downloadClient.h
	$(CXX) ${INCLUDE} ${CXXFLAGS} -c $< 

requestChannelEvent.o: requestChannelEvent.cc requestChannelEvent.h
	$(CXX) ${INCLUDE} ${CXXFLAGS} -c $< 

requestEvent.o: requestEvent.cc requestEvent.h
	$(CXX) ${INCLUDE} ${CXXFLAGS} -c $< 

responseEvent.o: responseEvent.cc responseEvent.h
	$(CXX) ${INCLUDE} ${CXXFLAGS} -c $< 

ackEvent.o: ackEvent.cc ackEvent.h
	$(CXX) ${INCLUDE} ${CXXFLAGS} -c $< 

dataEvent.o: dataEvent.cc dataEvent.h
	$(CXX) ${INCLUDE} ${CXXFLAGS} -c $<

exampleEvent.o: exampleEvent.cc exampleEvent.h
	$(CXX) ${INCLUDE} ${CXXFLAGS} -c $<

serverTransfer.o: serverTransfer.cc serverTransfer.h
	$(CXX) ${INCLUDE} ${CXXFLAGS} -c $< 

clientTransfer.o: clientTransfer.cc clientTransfer.h
	$(CXX) ${INCLUDE} ${CXXFLAGS} -c $< 

clean:
	rm -f *.o
	rm -f client
	rm -f server
