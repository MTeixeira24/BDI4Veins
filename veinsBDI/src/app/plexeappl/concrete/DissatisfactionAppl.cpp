/*
 * DissatisfactionAppl.cpp
 *
 *  Created on: 27/10/2018
 *      Author: miguel
 */

#include "DissatisfactionAppl.h"

Define_Module(DissatisfactionAppl);

DissatisfactionAppl::~DissatisfactionAppl() {
    if(myId == 0){
        if(stage != Stage::ENVIRONMENTAL){
            switch(stage){
            case Stage::INITIAL:
                throw cRuntimeError("INVALID END STATE - INITIAL");
                break;
            case Stage::JOINERS:
                throw cRuntimeError("INVALID END STATE - JOINERS");
                break;
            case Stage::NONE:
                throw cRuntimeError("INVALID END STATE- NONE");
                break;
            }
        }
        if(ballotsFinished < 3)
            throw cRuntimeError("INVALID BALLOT COUNT");
    }
    if(startInitialVote != NULL)
        cancelAndDelete(startInitialVote);
    if(callJoinersTimer != NULL)
        cancelAndDelete(callJoinersTimer);
}

void DissatisfactionAppl::initialize(int stage){
    RouteVotingAppl::initialize(stage);
    if(stage == 1 && getPlatoonRole() == PlatoonRole::LEADER){
        joinTimeout = new cMessage("joinTimeout");
    }
}

void DissatisfactionAppl::leaderInitialBehaviour(){
    startInitialVote = new cMessage("startInitialVote");
    scheduleAt(simTime() + 1.5, startInitialVote);
    stage = Stage::INITIAL;
}

void DissatisfactionAppl::handleRequestToJoinNegotiation(const RequestJoinPlatoonMessage* msg){
    int joinerId = msg->getVehicleId();
    //See if we already have this joiner in the stack
    std::vector<int>::iterator it = std::find(potentialJoiners.begin(), potentialJoiners.end(), joinerId);
    if(it == potentialJoiners.end()){
        potentialJoiners.push_back(joinerId);
    }
    //Send ack to stop the joiner from broadcasting requests
    sendAck(AckType::JOIN_REQUEST_RECEIVED, msg->getVehicleId());
}

void DissatisfactionAppl::handleSelfMsg(cMessage* msg){
    if(msg == callJoinersTimer){
        if(potentialJoiners.size() < 3){
            scheduleAt(simTime() + 1, callJoinersTimer);
            return;
        }
        delete msg;
        callJoinersTimer = NULL;
        cancelEvent(sendProposal);
        delete sendProposal;
        //sort the vector
        std::sort(potentialJoiners.begin(), potentialJoiners.end());
        //Notify the first joiner to enter the platoon
        callToJoin();
        negotiationState = VoteState::CHAIR_ELECTION_END;
    } else if (msg == startInitialVote){
        int joinerId = -1;
        BeliefModel mnv("start/vote/node");
        mnv.pushInt(&joinerId);
        double args = -1;
        //Warn the agent to remove a link from the previous path
        if(stage == Stage::ENVIRONMENTAL){
            args = 1;
        }
        mnv.pushDouble(&args);
        manager->sendInformationToAgents(myId, &mnv);
        cycle = VoteCycle::ROUTE_VOTE;
    } else if(msg == startSpeedVoteDelay && stage == Stage::ENVIRONMENTAL){
        //stage = Stage::NONE;
        BeliefModel mnv("start/vote/speed");
        //Set an identifier to remove potential speeds
        double arg = 1;
        mnv.pushDouble(&arg);
        manager->sendInformationToAgents(myId, &mnv);
        cycle = VoteCycle::SPEED_VOTE;
    }else if(msg == joinTimeout){
        callToJoin();
    } else{
        RouteVotingAppl::handleSelfMsg(msg);
    }
}

void DissatisfactionAppl::callToJoin(){
    int next = potentialJoiners.front();
    //Skip the voting and accept
    VotingAppl::sendVoteResults(1, next);
    election_data.currentResult = 1;
    election_data.joinerId = next;
    cancelEvent(joinTimeout);
    scheduleAt(simTime() + 2, joinTimeout);
}

void DissatisfactionAppl::handleEndOfVote(){
    Enter_Method_Silent();
    ballotsFinished++;
    if(stage == Stage::INITIAL){
        scheduleAt(simTime() + 3, sendProposal);
        callJoinersTimer = new cMessage("callJoinersTimer");
        scheduleAt(simTime() + 6, callJoinersTimer);
        stage = Stage::JOINERS;
    }else if(stage == Stage::JOINERS){
        //start a vote due to environmental changes
        scheduleAt(simTime() + 3, startInitialVote);
        stage = Stage::ENVIRONMENTAL;
    }
}

void DissatisfactionAppl::finalizeManeuver(int joinerId){
    std::cout << "CALLED FINALIZE" << std::endl;
    BeliefModel mnv;
    mnv.pushInt(&joinerId);
    potentialJoiners.erase(potentialJoiners.begin());
    if(potentialJoiners.size() > 0){
        mnv.setBelief("addmember");
        callToJoin();
    }
    else{
        cancelEvent(joinTimeout);
        delete joinTimeout;
        if(positionHelper->getPlatoonSize() != 9)
            throw cRuntimeError("INVALID SIZE");
        negotiationState = VoteState::CHAIR_ELECTION_ONGOING;
        mnv.setBelief("start/vote/node");
        double args = -1;
        mnv.pushDouble(&args);
        cycle = VoteCycle::ROUTE_VOTE;
    }
    manager->sendInformationToAgents(myId, &mnv);
}
