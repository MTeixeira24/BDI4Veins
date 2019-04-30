/*
 * RouterManager.cpp
 *
 *  Created on: 18/11/2018
 *      Author: miguel
 */

#include "../../managers/routerManager/RouterManager.h"

Define_Module(RouterManager);

void RouterManager::initialize(int stage){
    LightJasonManager::initialize(stage);
}

void RouterManager::parseResponse(uint32_t msgLength){
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
            case SET_NODE:
            {
                rbf >> type;
                ASSERT(type == VALUE_ARRAY);
                std::vector<int> votes = parseArrayMessage(rbf);
                /*rbf >> type;
                ASSERT(type == VALUE_INT);
                rbf >> leaderid;
                rbf >> type;
                ASSERT(type == VALUE_DOUBLE);
                rbf >> preferedspeed;
                rbf >> type;
                ASSERT(type == VALUE_DOUBLE);
                rbf >> tolerance;
                ((RouterAppl*)(vehicles[agentId]))->sendRequestToJoin(platoonid, leaderid, preferedspeed, tolerance);*/
                ((RouterAppl*)(vehicles[agentId]))->setTarget(votes);
                break;
            }
            default:
                break;
            }
        }
        queryMsg = new cMessage("query");
        scheduleAt(simTime() + updateInterval, queryMsg);
    }
}
