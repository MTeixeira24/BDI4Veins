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
    //We assume the sender already knows the id of the other leader.
    targetLeaderId = leaderRole == LeaderRole::SENDER ? 0 : -1;
}

void RegroupAppl::createRegroupElection(MemberExchange* me){
    std::vector<int> myCandidates = positionHelper->getPlatoonFormation();
    std::vector<int> myPrefSpeeds = ((VoteManager*)manager)->getElementsPreferredSpeed(myCandidates);
    BeliefModel startVote("start/vote/regroup");
    const uint32_t size = me->getIdMembersArraySize();
    std::vector<double> candidates(size);
    for(uint32_t i = 0; i < size; i++)
        candidates[i] = me->getIdMembers(i);
    std::vector<double> context(size);
    for(uint32_t i = 0; i < size; i++)
        context[i] = me->getSpeedMembers(i);

    //Receivers occupies left most side of the vectors
    int fullSize = myCandidates.size() + candidates.size();
    std::vector<double> fullCandidates(fullSize);
    std::vector<double> fullContext(fullSize);
    if(leaderRole == LeaderRole::RECEIVER){
        fullCandidates.insert(fullCandidates.end(), myCandidates.begin(), myCandidates.end());
        fullCandidates.insert(fullCandidates.end(), candidates.begin(), candidates.end());
        fullContext.insert(fullContext.end(), myPrefSpeeds.begin(), myPrefSpeeds.end());
        fullContext.insert(fullContext.end(), context.begin(), context.end());
    }else{
        fullCandidates.insert(fullCandidates.end(), candidates.begin(), candidates.end());
        fullCandidates.insert(fullCandidates.end(), myCandidates.begin(), myCandidates.end());
        fullContext.insert(fullContext.end(), context.begin(), context.end());
        fullContext.insert(fullContext.end(), myPrefSpeeds.begin(), myPrefSpeeds.end());
    }

    startVote.pushDoubleArray(candidates);
    startVote.pushDoubleArray(context);
    manager->sendInformationToAgents(myId, &startVote);
}

void RegroupAppl::sendDataExchange(RegroupMsgTypes msgType, int origin, int target, std::vector<int>& data){
    DataExchange* de = new DataExchange("DataExchange");
    de->setType((int)msgType);
    de->setDataArraySize(data.size());
    for(uint32_t i = 0; i < data.size(); i++)
        de->setData(i, data[i]);
    fillNegotiationMessage(de, origin, target);
    sendUnicast(de, target);
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
    fillNegotiationMessage(me, myId, targetLeaderId);
    sendUnicast(me, targetLeaderId);
}

void RegroupAppl::delegateNegotiationMessage(NegotiationMessage* nm){
    if(DataExchange* de = dynamic_cast<DataExchange*>(nm)){
        handleDataExchange(de);
    }else if(MemberExchange* me = dynamic_cast<MemberExchange*>(nm)){
        handleMemberExchange(me);
    }else
        VotingAppl::delegateNegotiationMessage(nm);
}

void RegroupAppl::handleDataExchange(DataExchange* de){
    if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::COMMITTEE_VOTE)){
        //We are still in a voting process. Save the message so we handle it when we are done.
        databuffer = de;
    }else if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::AWAITING_OTHER_RESULTS)){
        //We have all the data we need, call the winner determination module
        int size = databuffer->getDataArraySize();
        std::vector<int> totalVotes(size);
        for(int i = 0; i < size; i++)
            totalVotes[i] = databuffer->getData(i) + de->getData(i);
        BeliefModel bf("call/winner/determination");
        bf.pushIntArray(totalVotes);
        manager->sendInformationToAgents(myId, &bf);
        regroupState = RegroupState::AWAITING_WINNER_DETERMINATION;
        delete databuffer;
        databuffer = NULL;
        delete de;
    }else if((leaderRole == LeaderRole::SENDER) && (regroupState == RegroupState::AWAITING_WINNER_DETERMINATION)){
        //The receiver has sent over the data
        regroupState = RegroupState::COMMITTEE_VOTE_FINISHED;
        int size = de->getDataArraySize();
        std::vector<int> result(size);
        for(int i = 0; i < size; i++)
            result[i] = de->getData(i);
        sendCommitteeVoteResults(result);
        delete de;
    }
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
        targetLeaderId = me->getVehicleId();
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

void RegroupAppl::sendCommitteeVoteResults(std::vector<int>& results){
    if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::COMMITTEE_VOTE)){
        //The sender still hasnt sent over their results, store our results
        if(databuffer == NULL){
            regroupState = RegroupState::AWAITING_OTHER_RESULTS;
            databuffer = new DataExchange("Storage");
            databuffer->setDataArraySize(results.size());
            for(uint32_t i = 0; i < results.size(); i++)
                databuffer->setData(i, results[i]);
        }else{
            //The sender has sent over their votes, call the winner determination module
            int size = databuffer->getDataArraySize();
            std::vector<int> totalVotes(size);
            for(int i = 0; i < size; i++)
                totalVotes[i] = databuffer->getData(i) + results[i];
            BeliefModel bf("call/winner/determination");
            bf.pushIntArray(totalVotes);
            manager->sendInformationToAgents(myId, &bf);
            regroupState = RegroupState::AWAITING_WINNER_DETERMINATION;
            delete databuffer;
            databuffer = NULL;
        }
    }else if((leaderRole == LeaderRole::SENDER) && (regroupState == RegroupState::COMMITTEE_VOTE)){
        //Send over our votes
        sendDataExchange(RegroupMsgTypes::VOTE_DATA, myId, targetLeaderId, results);
        regroupState = RegroupState::AWAITING_WINNER_DETERMINATION;
    }else if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::AWAITING_WINNER_DETERMINATION)){
        //We got the results of the winner determination, notify all
        sendDataExchange(RegroupMsgTypes::ELECTION_RESULT, myId, targetLeaderId, results);
        regroupState = RegroupState::COMMITTEE_VOTE_FINISHED;
    }else {
        RouteVotingAppl::sendCommitteeVoteResults(results);
    }
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

void RegroupAppl::fillContextVector(const NotifyVote* msg, std::vector<double>& contextArgs){
    VotingAppl::fillContextVector(msg, contextArgs);
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
