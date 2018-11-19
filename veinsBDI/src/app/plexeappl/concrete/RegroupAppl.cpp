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

void RegroupAppl::createRegroupElection(MemberExchange* me){

}

void RegroupAppl::sendMemberExchange(RegroupMsgTypes type){
    MemberExchange* me = new MemberExchange("MemberExchange");
    me->setType((int)type);
    me->setPlatoonId(positionHelper->getPlatoonId());
    //For an ACK we dont need the extra information
    if(type != RegroupMsgTypes::ACK){
        std::vector<int> elements = positionHelper->getPlatoonFormation();
        std::vector<int> preferredSpeeds = ((VoteManager*)manager)->getElementsPreferredSpeed(elements);
        me->setIdMembersArraySize(elements.size());
        for(uint32_t i = 0; i < elements.size(); i++)
            me->setIdMembers(i, elements[i]);
        me->setSpeedMembersArraySize(preferredSpeeds.size());
        for(uint32_t i = 0; i < preferredSpeeds.size(); i++)
            me->setSpeedMembers(i, preferredSpeeds[i]);
    }
    sendUnicast(me, -1);
}

void RegroupAppl::delegateNegotiationMessage(NegotiationMessage* nm){
    if(DataExchange* de = dynamic_cast<DataExchange*>(nm)){
        handleDataExchange(de);
        delete de;
    }else if(MemberExchange* me = dynamic_cast<MemberExchange*>(nm)){
        handleMemberExchange(me);
    }else
        VotingAppl::delegateNegotiationMessage(nm);
}

void RegroupAppl::handleDataExchange(DataExchange* de){

}

void RegroupAppl::handleMemberExchange(MemberExchange* me){
    if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::NONE)){
        //Make sure that our election is over
        regroupState = RegroupState::AWAITING_ACK_ACCEPT;
        if(negotiationState == VoteState::CHAIR_ELECTION_END){
            sendMemberExchange(RegroupMsgTypes::OK);
        }
        //Store the message for later use.
        buffer = me;
        return;
    }else if((leaderRole == LeaderRole::SENDER) && (regroupState == RegroupState::REQUESTING_EXCHANGE)){
        sendMemberExchange(RegroupMsgTypes::ACK);
        regroupState = RegroupState::COMMITTEE_VOTE;
        createRegroupElection(me);
    }else if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::AWAITING_ACK_ACCEPT)){
        regroupState = RegroupState::COMMITTEE_VOTE;
        createRegroupElection(me);
    }
    delete me;
}

void RegroupAppl::handleEndOfVote(){
    Enter_Method_Silent();
    /*The first vote for speed has ended. The sender leader sends a request*/
    if((leaderRole == LeaderRole::SENDER) && (regroupState == RegroupState::NONE)){
        sendMemberExchange(RegroupMsgTypes::REQUEST);
    }
    if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::AWAITING_ACK_ACCEPT)){
        //Have we already received a regroupMessage?
        sendMemberExchange(RegroupMsgTypes::OK);
        createRegroupElection(buffer);
        delete buffer;
        buffer = NULL;
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
