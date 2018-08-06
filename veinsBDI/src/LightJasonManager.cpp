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
    listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerSocket == INVALID_SOCKET)
        throw cRuntimeError("cSocketRTScheduler: cannot create socket");

    sockaddr_in sinInterface;
    //addrinfo hints;
    sinInterface.sin_family = AF_INET;
    //hints.ai_family = AF_INET;
    //getaddrinfo("localhost", NULL, &hints, &infoptr);
    sinInterface.sin_addr.s_addr = inet_addr("127.0.0.1");
    sinInterface.sin_port = htons(4242);
    int n = connect(listenerSocket, (sockaddr *)&sinInterface, sizeof(sockaddr_in));
    /*if (bind(listenerSocket, (sockaddr *)&sinInterface, sizeof(sockaddr_in)) == SOCKET_ERROR)
        throw cRuntimeError("cSocketRTScheduler: socket bind() failed");

    listen(listenerSocket, SOMAXCONN);*/
    if (n < 0){
        throw cRuntimeError("cSocketRTScheduler: socket connect() failed");
     }
    char buffer[256] = "Is anybodythere?\n\0";
    n = write(listenerSocket,buffer,strlen(buffer));
    if (n < 0){
        throw cRuntimeError("cSocketRTScheduler: write failed");
    }
    bzero(buffer, 256);
    n = read(listenerSocket,buffer,255);
    if (n < 0){
        throw cRuntimeError("cSocketRTScheduler: read failed");
        }
    printf("%s\n",buffer);
    queryMsg = new cMessage("query");
    scheduleAt(simTime() + updateInterval, queryMsg);
}

void LightJasonManager::handleMessage(cMessage* msg){
    if (msg == queryMsg){
        delete queryMsg;
        std::string jasonQuery("Query-\n");
        char buffer[256];
        writeToSocket(jasonQuery, buffer);
        printf("%s\n",buffer);
        jasonQuery = buffer;
        if(jasonQuery.compare("Ok\n") != 0){
            size_t pos = 0, spos = 0;
            std::string token, stoken;
            while ((pos = jasonQuery.find(":")) != std::string::npos) {
                token = jasonQuery.substr(0, pos);
                std::cout << token << std::endl;
                /*while ((spos = token.find("-")) != std::string::npos) {
                    stoken = token.substr(0, spos);
                    std::cout << stoken << std::endl;
                    token.erase(0, spos + 1);
                }*/
                //TODO: Just to test commands from agents. Generalize into protocol
                spos = token.find("-");
                int m_id = std::stoi(token.substr(0, spos));
                token.erase(0, spos + 1);
                spos = token.find("-");
                token.erase(0, spos + 1);
                spos = token.find("-");
                double amount = std::stod(token.substr(0, spos));
                vehicles[m_id]->changeSpeed(amount);
                jasonQuery.erase(0, pos + 1);
            }
        }
        queryMsg = new cMessage("query");
        scheduleAt(simTime() + updateInterval, queryMsg);
    }
}

uint8_t LightJasonManager::subscribeVehicle(TutorialAppl* vehicle, uint32_t id){
    vehicles[id] = vehicle;
    std::string msg ("Add-");
    msg.append(std::to_string(id));
    msg.append("\n");
    char buffer[256];
    bcopy(msg.c_str(), buffer, strlen(msg.c_str()));
    int n = write(listenerSocket,buffer,strlen(buffer));
    if (n < 0){
        throw cRuntimeError("cSocketRTScheduler: write failed");
    }
    bzero(buffer, 256);
    n = read(listenerSocket,buffer,255);
    if (n < 0){
        throw cRuntimeError("cSocketRTScheduler: read failed");
        }
    printf("%s\n",buffer);
    return 0;
}
uint8_t LightJasonManager::sendInformationToAgents(int id, std::string belief, std::string value){
    if(belief.compare("speed") == 0){
        std::string msg("BeliefUpdate-");
        msg.append(std::to_string(id));
        msg.append("-speed-");
        msg.append(value);
        msg.append("\n");
        char buffer[256];
        writeToSocket(msg, buffer);
        printf("%s\n",buffer);
    }
    return 0;
}

int LightJasonManager::writeToSocket(std::string data, char* buffer){
    bcopy(data.c_str(), buffer, strlen(data.c_str()));
    int n = write(listenerSocket,buffer,strlen(buffer));
    if (n < 0){
        throw cRuntimeError("cSocketRTScheduler: write failed");
    }
    bzero(buffer, 256);
    n = read(listenerSocket,buffer,255);
    if (n < 0){
        throw cRuntimeError("cSocketRTScheduler: read failed");
        }
    return 0;
}



