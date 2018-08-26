/*
 * LightJasonManager.h
 *
 *  Created on: Aug 1, 2018
 *      Author: miguel
 */

#ifndef LIGHTJASONMANAGER_H_
#define LIGHTJASONMANAGER_H_
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <omnetpp/platdep/sockets.h>
#include <string.h>
#include <omnetpp.h>
#include <map>

#include "../../app/baseappl/BaseAgentAppl.h"
#include "JasoNetProtocol.h"
#include "LightJasonBuffer.h"
using namespace omnetpp;
class BaseAgentAppl;
class LightJasonManager : public cSimpleModule{
public:
    LightJasonManager();
    ~LightJasonManager();
    uint8_t subscribeVehicle(BaseAgentAppl* vehicle, uint32_t id);
    uint8_t sendInformationToAgents(int id, std::string belief, double value);
    void notifyNodes(uint32_t id, std::string action, std::string data);
    virtual void initialize(int stage) override;
    void unsubscribeVehicle(int id);
protected:
    int port;
    std::map <int, BaseAgentAppl*> vehicles;
    simtime_t updateInterval;
    cModule *module;
    cMessage *notificationMsg;
    cMessage *queryMsg;
    char *recvBuffer;
    int recvBufferSize;
    int *numBytesPtr;

    // state
    //int64_t baseTime; // in microseconds, as returned by opp_get_monotonic_clock_usecs()
    SOCKET connSocket;
    JasoNetProtocol jp;
    virtual void handleMessage(cMessage *msg) override;
    LightJasonBuffer writeToSocket(std::string data);
    uint32_t getMessageLength();
    LightJasonBuffer receiveMessage(uint32_t);

    //virtual void setupListener();
};



#endif /* LIGHTJASONMANAGER_H_ */
