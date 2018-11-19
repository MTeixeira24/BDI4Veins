/*
 * RegroupAppl.cpp
 *
 *  Created on: Nov 14, 2018
 *      Author: miguel
 */

#include "RegroupAppl.h"

Define_Module(RegroupAppl);

RegroupAppl::~RegroupAppl() {
}

void RegroupAppl::leaderInitialBehaviour(){
    RouteVotingAppl::leaderInitialBehaviour();
    /*
     * Right-most platoon has id of 0, left-most has id of 1
     * This scenario assumes that the left-most platoon starts the negotiation
     */
    leaderRole = positionHelper->getPlatoonId() == 0 ? LeaderRole::RECEIVER : LeaderRole::SENDER;
}

void RegroupAppl::delegateNegotiationMessage(NegotiationMessage* nm){
    if(DataExchange* de = dynamic_cast<DataExchange*>(nm)){
        handleDataExchange(de);
        delete de;
    }else if(MemberExchange* me = dynamic_cast<MemberExchange*>(nm)){
        handleMemberExchange(me);
        delete me;
    }else
        VotingAppl::delegateNegotiationMessage(nm);
}

void RegroupAppl::handleDataExchange(DataExchange* de){

}

void RegroupAppl::handleMemberExchange(MemberExchange* me){
    if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::NONE)){

    }else if((leaderRole == LeaderRole::SENDER) && (regroupState == RegroupState::REQUESTING_EXCHANGE)){

    }else if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::AWAITING_ACK_ACCEPT)){

    }
}

void RegroupAppl::handleEndOfVote(){
    Enter_Method_Silent();
    /*The first vote for speed has ended. The sender leader sends a request*/
    if((leaderRole == LeaderRole::SENDER) && (regroupState == RegroupState::NONE)){

    }
}

void RegroupAppl::handleSelfMsg(cMessage* msg){
    if(msg == startInitialVote){
        /*Vote only speeds. No need to vote on routes. That is verified in scenario 2*/
        int joinerId = -1;
        BeliefModel mnv("start/vote/speed");
        mnv.pushInt(&joinerId);
        double args = -1;
        mnv.pushDouble(&args);
        manager->sendInformationToAgents(myId, &mnv);
        cycle = VoteCycle::ROUTE_VOTE;
        negotiationState = VoteState::CHAIR_ELECTION_ONGOING;
    }  else{
        RouteVotingAppl::handleSelfMsg(msg);
    }
}
