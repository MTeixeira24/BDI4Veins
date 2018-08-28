/*
 * LightJasonManager.cpp
 *
 *  Created on: Aug 1, 2018
 *      Author: miguel
 */

#include "LightJasonManager.h"
Define_Module(LightJasonManager)
LightJasonManager::LightJasonManager(){

}

LightJasonManager::~LightJasonManager(){
}

void LightJasonManager::initialize(int stage){
    cSimpleModule::initialize(stage);
    updateInterval = par("updateInterval");
    connSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (connSocket == INVALID_SOCKET)
            throw cRuntimeError("LightJasonManager: cannot create socket");
    int n = -1;


    struct addrinfo *address, hints;
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_protocol = 0;
    hints.ai_socktype = SOCK_STREAM;
    n = getaddrinfo("localhost", "4242", &hints, &address); //TODO: Have this setting be determine by whether or not we are in a docker container https://forums.docker.com/t/localhost-and-docker-compose-networking-issue/23100
    if (n != 0) {
        if (n == EAI_SYSTEM) {
            throw cRuntimeError("LightJasonManager: EAI_SYSTEM error");
        } else {
            throw cRuntimeError(gai_strerror(n));
        }
        exit(EXIT_FAILURE);
    }

    for(int timeout = 0; timeout <= 10; timeout++){
        n = connect(connSocket, address->ai_addr, address->ai_addrlen);
        if (n >= 0) break;
        if(timeout == 10) throw cRuntimeError("LightJasonManager: socket connect() failed");
        sleep(1);
    }
    LightJasonBuffer res = writeToSocket(jp.connectionRequest().getBuffer());
    n = int((unsigned char)res.getBuffer()[0]);
    EV << n << "\n"; //DEBUG
    queryMsg = new cMessage("query");
    scheduleAt(simTime() + updateInterval, queryMsg);
}

void LightJasonManager::handleMessage(cMessage* msg){
    if (msg == queryMsg){
        delete queryMsg;
        int n = write(connSocket,jp.query().getBuffer().c_str(),255);
        if (n < 0){
            throw new cRuntimeError("LightJasonManager: handleMessage write error");
        }
        uint32_t msgLength = getMessageLength();
        if(msgLength > 0){
            parseResponse(msgLength);
        }else{
            queryMsg = new cMessage("query");
            scheduleAt(simTime() + updateInterval, queryMsg);
        }

    }
}

void LightJasonManager::notifyNodes(uint32_t id, std::string action, std::string data){
    if(action.compare("none") != 0){
        if(action.compare("setMaxSpeed") == 0){
            double amount = std::stod(data);
            vehicles[id]->changeSpeed(amount);
        }
    }
}

void LightJasonManager::parseResponse(uint32_t msgLength){
    msgLength -= sizeof(uint32_t);
    LightJasonBuffer rbf = receiveMessage(msgLength);
    uint16_t type;
    uint16_t commandId;
    rbf >> commandId;
    ASSERT(commandId == QUERY || commandId == TERMINATE_CONNECTION);
    if(commandId == TERMINATE_CONNECTION){
        close(connSocket);
    }else{
        while(!rbf.eof()){
            uint32_t agentMessageLength; //TODO: Organize this into structs
            rbf >> agentMessageLength;
            agentMessageLength -= sizeof(uint32_t);
            uint32_t agentId;
            rbf >> agentId;
            uint32_t agentAction;
            rbf >> agentAction;
            switch (agentAction){
            case SET_MAX_SPEED:
                rbf >> type;
                ASSERT(type == VALUE_DOUBLE);
                double speed;
                rbf >> speed;
                vehicles[agentId]->changeSpeed(speed);
                break;
            case REQUEST_SPEED_DOWN:
                rbf >> type;
                ASSERT(type == VALUE_INT);
                int id;
                rbf >> id;
                MessageParameters mp;
                mp.messageRequest = REQUEST_SPEED_DOWN;
                mp.targetId = id;
                vehicles[agentId]->sendMessage(MESSAGE_UNICAST, &mp);
                break;
            default:
                break;
            }
        }
        queryMsg = new cMessage("query");
        scheduleAt(simTime() + updateInterval, queryMsg);
    }
}

uint8_t LightJasonManager::subscribeVehicle(BaseAgentAppl* vehicle, uint32_t id, std::string vType, std::string aslFile){
    vehicles[id] = vehicle;
    std::string debug = jp.subscriptionRequest(id, vType, aslFile).getBuffer();
    LightJasonBuffer result = writeToSocket(jp.subscriptionRequest(id, vType, aslFile).getBuffer());
    return 0;
}

void LightJasonManager::unsubscribeVehicle(int id){
    LightJasonBuffer result = writeToSocket(jp.removeRequest(id).getBuffer());
    vehicles.erase(id);
}

uint8_t LightJasonManager::sendInformationToAgents(int id, std::string belief, double value){
    LightJasonBuffer result = writeToSocket(jp.buildUpdateBeliefQuery(id, belief, value).getBuffer());
    int n = int((unsigned char)result.getBuffer()[0]);
    EV << n << "\n"; //DEBUG
    return 0;
}

LightJasonBuffer LightJasonManager::writeToSocket(std::string data){
    int n = write(connSocket,data.c_str(),data.length());
    if (n < 0){
        throw cRuntimeError("LightJasonManager: write failed");
    }
    uint32_t msgLength = getMessageLength();
    msgLength -= sizeof(uint32_t);
    return receiveMessage(msgLength);
}

uint32_t LightJasonManager::getMessageLength(){
    uint32_t msgLength;
    char lengthBuffer [sizeof(uint32_t)];
    int n = recv(connSocket,lengthBuffer,sizeof(uint32_t),0);
    if (n < 0){
        throw new cRuntimeError("LightJasonManager: handleMessage recv error");
    }
    LightJasonBuffer(std::string(lengthBuffer, sizeof(uint32_t))) >> msgLength;
    return msgLength;
}

LightJasonBuffer LightJasonManager::receiveMessage(uint32_t length){
    char msgBuffer[length];
    int n = recv(connSocket, msgBuffer, length,0);
    if (n < 0){
        throw new cRuntimeError("LightJasonManager: handleMessage recv error");
    }
    return LightJasonBuffer(std::string(msgBuffer, length));
}



