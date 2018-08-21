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
        throw cRuntimeError("cSocketRTScheduler: cannot create socket");
    sockaddr_in sinInterface;
    sinInterface.sin_family = AF_INET;
    sinInterface.sin_addr.s_addr = inet_addr("127.0.0.1"); //TODO: Have this setting be determine by whether or not we are in a docker container https://forums.docker.com/t/localhost-and-docker-compose-networking-issue/23100
    sinInterface.sin_port = htons(4242);
    int n;
    for(int timeout = 0; timeout <= 10; timeout++){
        n = connect(connSocket, (sockaddr *)&sinInterface, sizeof(sockaddr_in));
        if (n >= 0) break;
        if(timeout == 10) throw cRuntimeError("LightJasonManager: socket connect() failed");
        sleep(1);
    }
    std::string msg = jp.buildConnectionRequest();
    std::string result = writeToSocket(msg);
    EV << result << "\n";
    queryMsg = new cMessage("query");
    scheduleAt(simTime() + updateInterval, queryMsg);
}

void LightJasonManager::handleMessage(cMessage* msg){
    if (msg == queryMsg){
        delete queryMsg;
        std::string jasonQuery("Query-\n");
        std:: string result = writeToSocket(jasonQuery);
        EV << result;
        uint32_t id;
        std::string data;
        std::string action;
        data = jp.parseResponse(result, &id, action);
        if(action.compare("EndConnection") == 0){
            close(connSocket);
        }else{
            notifyNodes(id, action, data);
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

uint8_t LightJasonManager::subscribeVehicle(AgentAppl* vehicle, uint32_t id){
    vehicles[id] = vehicle;
    std::string msg = jp.buildSubscriptionRequest(id);
    std::string result = writeToSocket(msg);
    EV << result;
    return 0;
}

void LightJasonManager::unsubscribeVehicle(int id){
    std::string msg = jp.buildRemoveRequest(id);
    std:: string result = writeToSocket(msg);
    vehicles.erase(id);
    EV << result;
}

uint8_t LightJasonManager::sendInformationToAgents(int id, std::string belief, std::string value){
    std::string msg = jp.buildBeliefUpdateRequest(id, belief, value);
    std:: string result = writeToSocket(msg);
    EV << result;
    return 0;
}

std::string LightJasonManager::writeToSocket(std::string data){
    int n = write(connSocket,data.c_str(),strlen(data.c_str()));
    if (n < 0){
        throw cRuntimeError("cSocketRTScheduler: write failed");
    }
    char buffer[256];
    bzero(buffer, 256);
    n = read(connSocket,buffer,255);
    if (n < 0){
        throw cRuntimeError("cSocketRTScheduler: read failed");
    }
    return std::string(buffer);
}



