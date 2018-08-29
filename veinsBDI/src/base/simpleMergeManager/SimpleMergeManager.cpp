/*
 * SimpleMergeManager.cpp
 *
 *  Created on: 29/08/2018
 *      Author: miguel
 */

#include "SimpleMergeManager.h"

Define_Module(SimpleMergeManager)

SimpleMergeManager::SimpleMergeManager() {
}

SimpleMergeManager::~SimpleMergeManager() {
}

void SimpleMergeManager::initialize(int stage){
    LightJasonManager::initialize(stage);
}

void SimpleMergeManager::parseResponse(uint32_t msgLength){
    msgLength -= sizeof(uint32_t);
    LightJasonBuffer rbf = receiveMessage(msgLength);
    uint16_t type;
    uint16_t commandId;
    rbf >> commandId;
    ASSERT(commandId == QUERY || commandId == TERMINATE_CONNECTION);
    //SimpleMerge* test = (SimpleMerge*)vehicles[0];
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
            /*case SET_MAX_SPEED:
                rbf >> type;
                ASSERT(type == VALUE_DOUBLE);
                double speed;
                rbf >> speed;
                vehicles[agentId]->changeSpeed(speed);
                break;*/
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
