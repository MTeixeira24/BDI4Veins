/*
 * LightJasonManager.cpp
 *
 *  Created on: Aug 1, 2018
 *      Author: miguel
 */

#include "../../managers/manager/LightJasonManager.h"

Define_Module(LightJasonManager)

LightJasonManager::LightJasonManager(){
    bulkAddInitialized = false;
    bulkAddSent = false;
    vehiclesAwaitingSubscription = 0;
    triggerQueue.reserve(1024*1024);
}

LightJasonManager::~LightJasonManager(){
    cancelAndDelete(queryMsg);
    close(connSocket);
}

void LightJasonManager::finish(){
    //writeToSocket(jp.terminateConnection().getBuffer());
}

void LightJasonManager::initialize(int stage){
    cSimpleModule::initialize(stage);
    if(stage == 0){
        useBulkInsert = par("useBulkAgentCreation").boolValue();
        updateInterval = par("updateInterval");
        vehicleCount = par("vehicleCount").intValue();
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

        setLightJasonParameters();
    }
}

void LightJasonManager::setLightJasonParameters(){
    LightJasonBuffer buff;
    jp.initializeParamsBuffer(buff);
    jp.setSimParameters(buff, par("agentManager").stdstringValue());
    writeToSocket(buff.getBuffer());
}

void LightJasonManager::handleMessage(cMessage* msg){
    if (msg == queryMsg){

        sendTriggers();

        scheduleAt(simTime() + updateInterval, queryMsg);
    }
}

BaseAgentAppl* LightJasonManager::getVehicle(int id){
    return vehicles[id];
}

void LightJasonManager::parseResponse(LightJasonBuffer rbf, uint32_t msgLength){
}

void LightJasonManager::parseResponse(uint32_t msgLength){
    msgLength -= sizeof(uint32_t);
    LightJasonBuffer rbf = receiveMessage(msgLength);
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
            default:
                break;
            }
        }
    }
}

void LightJasonManager::QueueTrigger(Trigger& trigger){
    if(triggerQueue.isEmpty()){
        //append the message type
        triggerQueue << 6 << (uint16_t)SET_BELIEF;
    }
    trigger.convertToMessage(triggerQueue);

//    LightJasonBuffer result = writeToSocket(jp.buildAddGoalQuery(id, beliefModel).getBuffer());
//    int n = int((unsigned char)result.getBuffer()[0]);
//    EV << n << "\n"; //DEBUG
//    return 0;
}

void LightJasonManager::sendTriggers(){
    if(!triggerQueue.isEmpty()){
        //append the end of message identifier
        triggerQueue << 0xFFFF;
        //send the message
        //LightJasonBuffer result = writeToSocket(triggerQueue.getBuffer());
        int n = write(connSocket,triggerQueue.getBuffer().c_str(),triggerQueue.length());
        if (n < 0){
            throw new cRuntimeError("LightJasonManager: handleMessage write error");
        }
        uint32_t msgLength = getMessageLength();
        if(msgLength > 0){
            msgLength -= sizeof(uint32_t);
            parseResponse(receiveMessage(msgLength), msgLength);
        }
        triggerQueue.clear();
    }
}

void LightJasonManager::initializeTriggerMap(LightJasonBuffer &data){
    int triggerId;
    std::string triggerName;
    while(!data.eof()){
        data >> triggerId;
        data >> triggerName;
        triggerMap[triggerName] = triggerId;
    }
}

uint8_t LightJasonManager::getBeliefId(std::string key){
    if(triggerMap.find(key) != triggerMap.end())
        return triggerMap[key];
    else return 0xFF;
}

uint8_t LightJasonManager::subscribeVehicle(BaseAgentAppl* vehicle, uint32_t id, std::string vType, std::string aslFile){
    if(useBulkInsert){
        if(!bulkAddInitialized){
            bulkAddInstruction = jp.initializeBulkSubscriptionRequest();
            bulkAddInitialized = true;
        }
        vehicles[id] = vehicle;
        ++vehiclesAwaitingSubscription;
        jp.addToBulkSubscriptionRequest(bulkAddInstruction, id, vType, aslFile);
        if(vehiclesAwaitingSubscription == vehicleCount){
            jp.terminateBulkSubscriptionRequest(bulkAddInstruction);
            LightJasonBuffer result = writeToSocket(bulkAddInstruction.getBuffer());
            bulkAddInitialized = false;
            bulkAddSent = true;
            initializeTriggerMap(result);
        }
        return 0;
    }else{
        vehicles[id] = vehicle;
        std::string debug = jp.subscriptionRequest(id, vType, aslFile).getBuffer();
        LightJasonBuffer result = writeToSocket(jp.subscriptionRequest(id, vType, aslFile).getBuffer());
        return 0;
    }
}

void LightJasonManager::unsubscribeVehicle(int id){
    LightJasonBuffer result = writeToSocket(jp.removeRequest(id).getBuffer());
    vehicles.erase(id);
}

//uint8_t LightJasonManager::sendInformationToAgents(int id, const void* beliefModel){
//    LightJasonBuffer result = writeToSocket(jp.buildAddGoalQuery(id, beliefModel).getBuffer());
//    int n = int((unsigned char)result.getBuffer()[0]);
//    EV << n << "\n"; //DEBUG
//    return 0;
//}

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
    //char msgBuffer[length];
    int n = recv(connSocket, msgBuffer, USHRT_MAX,0);
    if (n < 0){
        throw new cRuntimeError("LightJasonManager: handleMessage recv error");
    }
    return LightJasonBuffer(std::string(msgBuffer, length));
}

std::vector<int> LightJasonManager::parseArrayMessage(LightJasonBuffer& buffer){
    short arrayType;
    buffer >> arrayType;
    ASSERT(arrayType == VALUE_INT);
    uint32_t size;
    buffer >> size;
    std::vector<int> contents(size);
    int element;
    for(uint32_t i = 0; i < size; i++){
        buffer >> element;
        contents[i] = element;
    }
    return contents;
}

std::vector<double> LightJasonManager::parseDoubleArrayMessage(LightJasonBuffer& buffer){
    short arrayType;
    buffer >> arrayType;
    ASSERT(arrayType == VALUE_DOUBLE);
    uint32_t size;
    buffer >> size;
    std::vector<double> contents(size);
    double element;
    for(uint32_t i = 0; i < size; i++){
        buffer >> element;
        contents[i] = element;
    }
    return contents;
}


