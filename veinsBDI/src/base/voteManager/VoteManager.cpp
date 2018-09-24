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
            int platoonid;
            int leaderid;
            double preferedspeed;
            double tolerance;
            switch (agentAction){
            case SEND_JOIN_REQUEST:
            {
                rbf >> type;
                ASSERT(type == VALUE_INT);
                rbf >> platoonid;
                rbf >> type;
                ASSERT(type == VALUE_INT);
                rbf >> leaderid;
                rbf >> type;
                ASSERT(type == VALUE_DOUBLE);
                rbf >> preferedspeed;
                rbf >> type;
                ASSERT(type == VALUE_DOUBLE);
                rbf >> tolerance;
                ((VotingAppl*)(vehicles[agentId]))->sendRequestToJoin(platoonid, leaderid, preferedspeed, tolerance);
                break;
            }
            case SUBMIT_VOTE:
            {
                rbf >> type;
                ASSERT(type == VALUE_ARRAY);
                std::vector<int> votes = parseArrayMessage(rbf);
                ((VotingAppl*)(vehicles[agentId]))->sendVoteSubmition(votes);
                break;
            }
            case SEND_VOTE_RESULTS:
            {
                int externalId; //If the negotiation involves external participants.
                rbf >> type;
                rbf >> externalId;
                if(externalId == -1){
                    rbf >> type;
                    ASSERT(type == VALUE_INT);
                    int joinerId;
                    rbf >>joinerId;
                    rbf >> type;
                    ASSERT(type == VALUE_INT);
                    int result;
                    rbf >>result;
                    ((VotingAppl*)(vehicles[agentId]))->sendVoteResults(joinerId, result);
                }else{
                    rbf >> type;
                    ASSERT(type == VALUE_INT);
                    int winnerValue;
                    rbf >> winnerValue;
                    ((VotingAppl*)(vehicles[agentId]))->sendVoteResults(winnerValue);
                }
                break;
            }
            case NOTIFY_START_VOTE:
            {
                /*get the context identifier*/
                rbf >> type;
                ASSERT(type == VALUE_INT);
                int contextId;
                rbf >> contextId;
                /*get the context arguments*/
                rbf >> type;
                ASSERT((type == VALUE_NULL) || (type == VALUE_ARRAY));
                std::vector<double> contextArgs;
                if(type == VALUE_ARRAY){
                    contextArgs = parseDoubleArrayMessage(rbf);
                }
                /*get the candidates array*/
                rbf >> type;
                ASSERT(type == VALUE_ARRAY);
                std::vector<int> candidates = parseArrayMessage(rbf);
                ((VotingAppl*)(vehicles[agentId]))->sendNotificationOfVote(contextId, contextArgs, candidates);
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
