/*
 * DissatisfactionAppl.cpp
 *
 *  Created on: 27/10/2018
 *      Author: miguel
 */

#include "DissatisfactionAppl.h"

Define_Module(DissatisfactionAppl);

DissatisfactionAppl::~DissatisfactionAppl() {
}

void DissatisfactionAppl::initialize(int stage){
    RouteVotingAppl::initialize(stage);
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
        delete msg;
        callJoinersTimer = NULL;
        //sort the vector
        std::sort(potentialJoiners.begin(), potentialJoiners.end());
        //Notify the first joiner to enter the platoon
        callToJoin();
    } else if (msg == startInitialVote){
        delete msg;
        startInitialVote = NULL;
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
        delete msg;
        startSpeedVoteDelay = NULL;
        stage = Stage::NONE;
        BeliefModel mnv("start/vote/speed");
        //Set an identifier to remove potential speeds
        double arg = 1;
        mnv.pushDouble(&arg);
        manager->sendInformationToAgents(myId, &mnv);
        cycle = VoteCycle::SPEED_VOTE;
    } else{
        RouteVotingAppl::handleSelfMsg(msg);
    }
}

void DissatisfactionAppl::callToJoin(){
    int next = potentialJoiners.front();
    potentialJoiners.erase(potentialJoiners.begin());
    //Skip the voting and accept
    VotingAppl::sendVoteResults(1, next);
}

void DissatisfactionAppl::handleEndOfVote(){
    Enter_Method_Silent();
    if(stage == Stage::INITIAL){
        sendProposal = new cMessage("sendProposal");
        scheduleAt(simTime() + 3, sendProposal);
        callJoinersTimer = new cMessage("callJoinersTimer");
        scheduleAt(simTime() + 6, callJoinersTimer);
        negotiationState = VoteState::CHAIR_SEARCHING_JOINERS;
        stage = Stage::JOINERS;
    }else if(stage == Stage::JOINERS){
        //start a vote due to environmental changes
        startInitialVote = new cMessage("startInitialVote");
        scheduleAt(simTime() + 12, startInitialVote);
        stage = Stage::ENVIRONMENTAL;
    }
}

void DissatisfactionAppl::finalizeManeuver(int joinerId){
    BeliefModel mnv;
    mnv.pushInt(&joinerId);
    if(potentialJoiners.size() > 0){
        mnv.setBelief("addmember");
        callToJoin();
    }
    else{
        mnv.setBelief("start/vote/node");
        double args = -1;
        mnv.pushDouble(&args);
        cycle = VoteCycle::ROUTE_VOTE;
    }
    manager->sendInformationToAgents(myId, &mnv);
}