/*
 * LightJasonManager.h
 *
 *  Created on: Aug 1, 2018
 *      Author: miguel
 */

/*
 * Omnet module that establishes connection with the LightJason server and acts as a mediator between for the vehicle applications
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
#include "../../utilities/Timer.h"
#include "JasoNetProtocol.h"
#include "LightJasonBuffer.h"
using namespace omnetpp;
using namespace Jason;

struct MessageParameters {
    int platoonId;
    int leaderId;
    int targetId;
    int messageRequest;
};


class BaseAgentAppl;
class LightJasonManager : public cSimpleModule{
public:
    LightJasonManager();
    ~LightJasonManager();
    /*Subscribe the vehicle to the manager and send a request to LightJason to create an agent*/
    uint8_t subscribeVehicle(BaseAgentAppl* vehicle, uint32_t id, std::string vType, std::string aslFile);
    /*Update belief base of the associated agent*/
    uint8_t sendInformationToAgents(int, const void*);//(int id, std::string belief, double value);
    /*DEPRECATED: delegate decisions to agents*/
    void notifyNodes(uint32_t id, std::string action, std::string data);
    /*Perform initialization and establish connection with LightJason server*/
    virtual void initialize(int stage) override;
    /*Remove vehicle from subscription and request agent deletion on server*/
    void unsubscribeVehicle(int id);
    /**/
    BaseAgentAppl* getVehicle(int id);
protected:
    /*Port number for Server */
    int port;
    /*Map of all subscribed vehicle IDs and respective pointers*/
    std::map <int, BaseAgentAppl*> vehicles;


    /*Time interval in which to request decisions from server*/
    simtime_t updateInterval;
    //cModule *module;
    //cMessage *notificationMsg;
    /*Self message to time queries to server*/
    cMessage *queryMsg;
    //char *recvBuffer;
    //int recvBufferSize;
    //int *numBytesPtr;

    // state
    //int64_t baseTime; // in microseconds, as returned by opp_get_monotonic_clock_usecs()
    /*Connection socket descriptor*/
    SOCKET connSocket;
    /*Message builder*/
    JasoNetProtocol jp;
    /*Handle messages received from omnet*/
    virtual void handleMessage(cMessage *msg) override;
    /*Writes data to socket and returns buffer with message content*/
    LightJasonBuffer writeToSocket(std::string data);
    /*Returns message length from socket*/
    uint32_t getMessageLength();
    /*Returns buffer of size N containing message from socket*/
    LightJasonBuffer receiveMessage(uint32_t);
    /*Parse response received from lightjason server*/
    virtual void parseResponse(uint32_t msgLength);

    //virtual void setupListener();

    /**
     * Extracts the array from a buffer to a vector
     */
    virtual std::vector<int> parseArrayMessage(LightJasonBuffer& buffer);
    /**
     * Extract an array with double values from a buffer to a vector
     */
    virtual std::vector<double> parseDoubleArrayMessage(LightJasonBuffer& buffer);

    virtual void finish() override;
private:
    //Timer timer;
};



#endif /* LIGHTJASONMANAGER_H_ */
