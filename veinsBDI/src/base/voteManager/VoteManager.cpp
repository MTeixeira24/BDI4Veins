/*
 * VoteManager.cpp
 *
 *  Created on: 12/09/2018
 *      Author: miguel
 */

#include "VoteManager.h"

Define_Module(VoteManager);

VoteManager::VoteManager() {

}

VoteManager::~VoteManager() {

}

void VoteManager::initialize(int stage){
    LightJasonManager::initialize(stage);
}

void VoteManager::parseResponse(uint32_t msgLength){
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
            case SEND_JOIN_REQUEST:
                rbf >> type;
                ASSERT(type == VALUE_INT);
                int platoonid;
                int leaderid;
                double preferedspeed;
                rbf >> platoonid;
                rbf >> type;
                ASSERT(type == VALUE_INT);
                rbf >> leaderid;
                rbf >> type;
                ASSERT(type == VALUE_DOUBLE);
                rbf >> preferedspeed;
                ((VotingAppl*)(vehicles[agentId]))->sendRequestToJoin(platoonid, leaderid, preferedspeed);
                break;
            default:
                break;
            }
        }
        queryMsg = new cMessage("query");
        scheduleAt(simTime() + updateInterval, queryMsg);
    }
}
