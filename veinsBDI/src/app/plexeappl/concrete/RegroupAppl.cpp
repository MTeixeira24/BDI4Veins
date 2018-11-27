/*
 * RegroupAppl.cpp
 *
 *  Created on: Nov 14, 2018
 *      Author: miguel
 */

#include "RegroupAppl.h"

Define_Module(RegroupAppl);

RegroupAppl::~RegroupAppl() {
    if(regroupDelay != NULL){
        delete regroupDelay;
    }
}

void RegroupAppl::leaderInitialBehaviour(){
    RouteVotingAppl::leaderInitialBehaviour();
    /*
     * Right-most platoon has id of 0, left-most has id of 1
     * This scenario assumes that the left-most platoon starts the negotiation
     */
    leaderRole = positionHelper->getPlatoonId() == 0 ? LeaderRole::RECEIVER : LeaderRole::SENDER;
    //We assume the sender already knows the id of the other leader.
    targetLeaderId = leaderRole == LeaderRole::SENDER ? 0 : -1; //DEBUG STATE 1
    regroupDelay = new cMessage("regroupDelay");
}

void RegroupAppl::createRegroupElection(MemberExchange* me){
    std::vector<int> myCandidates = positionHelper->getPlatoonFormation();
    std::vector<int> myPrefSpeeds = ((VoteManager*)manager)->getElementsPreferredSpeed(myCandidates);
    const uint32_t size = me->getIdMembersArraySize();
    std::vector<double> candidates(size);
    for(uint32_t i = 0; i < size; i++)
        candidates[i] = me->getIdMembers(i);
    std::vector<double> context(size);
    for(uint32_t i = 0; i < size; i++)
        context[i] = me->getSpeedMembers(i);

    //Receivers occupies left most side of the vectors
    uint32_t fullSize = myCandidates.size() + candidates.size();
    std::vector<double> fullCandidates;
    std::vector<double> fullContext;
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
    //Check for valid constructions
    for(uint32_t i = 0; i < fullSize; i++){
        ASSERT(fullCandidates[i] == i);
        ASSERT(fullContext[i] == ((VoteManager*)manager)->getPreferredSpeed(fullCandidates[i]));
    }
    BeliefModel startVote("start/vote/regroup");
    startVote.pushDoubleArray(fullCandidates);
    startVote.pushDoubleArray(fullContext);
    manager->sendInformationToAgents(myId, &startVote);
}

void RegroupAppl::sendVoteSubmition(std::vector<int>& votes){
    if((leaderRole == LeaderRole::SENDER) && (regroupState == RegroupState::COMMITTEE_VOTE)){
        databuffer = new DataExchange("dataExchange");
        //databuffer->setDataMatrix(std::vector<std::vector<int>>(positionHelper->getPlatoonSize()));
        databuffer->setVotesBuffer(std::list<int>(votes.begin(), votes.end()));
        //The amount of votes to be held
        databuffer->getVotesBuffer().push_front(positionHelper->getPlatoonSize());
        //databuffer->getDataMatrix().push_back(votes);
        fillNegotiationMessage(databuffer, myId, targetLeaderId);
        databuffer->setPlatoonId(positionHelper->getPlatoonId());
    }else{
        RouteVotingAppl::sendVoteSubmition(votes);
    }
}

/*void RegroupAppl::sendDataExchange(RegroupMsgTypes msgType, int origin, int target, std::vector<int>& data){
    DataExchange* de = new DataExchange("DataExchange");
    de->setType((int)msgType);
    de->setDataArraySize(data.size());
    for(uint32_t i = 0; i < data.size(); i++)
        de->setData(i, data[i]);
    fillNegotiationMessage(de, origin, target);
    sendUnicast(de, target);
}*/

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

void RegroupAppl::sendCommitteeVoteResults(std::vector<int>& results){
    if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::COMMITTEE_VOTE)){ //DEBUG STATE 6
        //The sender still hasnt sent over their results, store our results
        if(databuffer == NULL){
            regroupState = RegroupState::AWAITING_OTHER_RESULTS;
            databuffer = new DataExchange("Storage");
            databuffer->setDataArraySize(results.size());
            for(uint32_t i = 0; i < results.size(); i++)
                databuffer->setData(i, results[i]);
        }else{
            //The sender has sent over their votes, call the winner determination module
            /*int size = databuffer->getDataArraySize();
            std::vector<int> totalVotes(size);
            for(int i = 0; i < size; i++)
                totalVotes[i] = databuffer->getData(i) + results[i];*/
            std::vector<int> extraVotes(databuffer->getVotesBuffer().begin(), databuffer->getVotesBuffer().end());
            BeliefModel bf("call/winner/determination");
            bf.pushIntArray(extraVotes);
            regroupState = RegroupState::AWAITING_WINNER_DETERMINATION;
            manager->sendInformationToAgents(myId, &bf);
            delete databuffer;
            databuffer = NULL;
        }
    }else if((leaderRole == LeaderRole::SENDER) && (regroupState == RegroupState::COMMITTEE_VOTE)){
        //Send over our votes
        ASSERT(false);
        //sendDataExchange(RegroupMsgTypes::VOTE_DATA, myId, targetLeaderId, results);
        regroupState = RegroupState::AWAITING_WINNER_DETERMINATION;
    }else if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::AWAITING_WINNER_DETERMINATION)){
        //We got the results of the winner determination, notify all
        //sendDataExchange(RegroupMsgTypes::ELECTION_RESULT, myId, targetLeaderId, results);
        regroupState = RegroupState::COMMITTEE_VOTE_FINISHED;
        std::vector<int> g2 = vectorDiff(election_data.candidates, results);
        sendRegroupResults(results, g2, results[0], g2[0]);
        //RouteVotingAppl::sendCommitteeVoteResults(results);
    }else {
        RouteVotingAppl::sendCommitteeVoteResults(results);
    }
}

void RegroupAppl::sendRegroupResults(std::vector<int>& p1, std::vector<int>& p2, int l1, int l2){
    RegroupMessage* rm = new RegroupMessage("RegroupMessage");
    rm->setLeader1(l1);
    rm->setLeader2(l2);
    rm->setPlatoon1(p1);
    rm->setPlatoon2(p2);
    fillNegotiationMessage(rm, myId, -1);
    sendUnicast(rm, -1);
    positionHelper->setIsLeader(false);
    //have the sender also process the results
    handleRegroupResults(rm);
}

std::vector<int> RegroupAppl::vectorDiff(const std::vector<int>& v1, const std::vector<int>& v2){
    std::vector<int> diff;
    std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(),
                          std::inserter(diff, diff.begin()));
    return diff;
}

void RegroupAppl::delegateNegotiationMessage(NegotiationMessage* nm){
    if(DataExchange* de = dynamic_cast<DataExchange*>(nm)){
        handleDataExchange(de);
    }else if(MemberExchange* me = dynamic_cast<MemberExchange*>(nm)){
        handleMemberExchange(me);
    }else if(RegroupMessage* rm = dynamic_cast<RegroupMessage*>(nm)){
        handleRegroupResults(rm);
        delete rm;
    }else {
        RouteVotingAppl::delegateNegotiationMessage(nm);
    }
}

void RegroupAppl::handleRegroupResults(RegroupMessage* msg){
    if(leaderRole == LeaderRole::SENDER){
        ASSERT(regroupState == RegroupState::AWAITING_WINNER_DETERMINATION);
        regroupState = RegroupState::COMMITTEE_VOTE_FINISHED;
        positionHelper->setIsLeader(false);
    }
    auto find = std::find(std::begin(msg->getPlatoon1()), std::end(msg->getPlatoon1()), myId);
    if(find != std::end(msg->getPlatoon1())){
        regroup(msg->getPlatoon1(), msg->getLeader1());
    }else{
        regroup(msg->getPlatoon2(), msg->getLeader2());
    }
}

void RegroupAppl::regroup(std::vector<int>& p, int l){
    positionHelper->setPlatoonFormation(p);
    positionHelper->setLeaderId(l);
    positionHelper->setPlatoonId(l);
    BeliefModel pbelief("set/initial/beliefs");
    pbelief.pushInt(&l);
    pbelief.pushInt(&l);
    int pspeed = positionHelper->getPlatoonSpeed() * 3.6;
    pbelief.pushInt(&pspeed);
    int chair = -1;
    if(l == myId){
        positionHelper->setIsLeader(true);
        for (uint32_t i = 0; i < p.size(); i++){
            received_votes[p[i]] = false;
        }
        if(startInitialVote == NULL)
            startInitialVote = new cMessage("startInitialVote");
        if(voteTimer == NULL)
            voteTimer = new cMessage("VoteTimerA");
        scheduleAt(simTime() + 0.5, startInitialVote);
        pbelief.pushInt(&l);
    }else{
        pbelief.pushInt(&chair);
    }
    pbelief.pushIntArray(p);
    manager->sendInformationToAgents(myId, &pbelief);
    cancelEvent(awaitAckTimer);
}

void RegroupAppl::handleDataExchange(DataExchange* de){
    if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::COMMITTEE_VOTE)){
        //We are still in a voting process. Save the message so we handle it when we are done.
        databuffer = de->dup(); //DEBUG STATE 7
    }else if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::AWAITING_OTHER_RESULTS)){
        //We have all the data we need, call the winner determination module
        /*int size = databuffer->getDataArraySize();
        std::vector<int> totalVotes(size);
        for(int i = 0; i < size; i++)
            totalVotes[i] = databuffer->getData(i) + de->getData(i);*/
        std::vector<int> extraVotes(de->getVotesBuffer().begin(), de->getVotesBuffer().end()); //DEBUG STATE 7
        BeliefModel bf("call/winner/determination");
        bf.pushIntArray(extraVotes);
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
        delete databuffer;
        databuffer = NULL;
    }
}

void RegroupAppl::handleMemberExchange(MemberExchange* me){
    if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::NONE)){
        ASSERT(me->getType() == (int)RegroupMsgTypes::REQUEST); //DEBUG STATE 4
        //Make sure that our election is over
        regroupState = RegroupState::AWAITING_ACK_ACCEPT;
        targetLeaderId = me->getVehicleId();
        if(negotiationState == VoteState::CHAIR_ELECTION_END){
            sendMemberExchange(RegroupMsgTypes::OK);
        }
        //Store the message for later use.
        buffer = me->dup();
        return;
    }else if((leaderRole == LeaderRole::SENDER) && (regroupState == RegroupState::REQUESTING_EXCHANGE)){
        ASSERT(me->getType() == (int)RegroupMsgTypes::OK);
        sendMemberExchange(RegroupMsgTypes::ACK);
        regroupState = RegroupState::COMMITTEE_VOTE;
        createRegroupElection(me); //DEBUG STATE 5
    }else if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::AWAITING_ACK_ACCEPT)){
        //The sender got our message, carry on with the regroup election
        ASSERT(me->getType() == (int)RegroupMsgTypes::ACK);
        regroupState = RegroupState::COMMITTEE_VOTE;
        createRegroupElection(buffer); //DEBUG STATE 5
    }
    delete me;
}

void RegroupAppl::handleSubmitVote(const SubmitVote* msg){
    if((leaderRole == LeaderRole::SENDER) && (regroupState == RegroupState::COMMITTEE_VOTE)){ //DEBUG STATE 6
        //The sender proxies the vote to the other platoon leader

        if(msg->getPlatoonId() != positionHelper->getPlatoonId()) return;
        if(myId != positionHelper->getLeaderId()) return;
        int size = msg->getVotesArraySize();
        if(size > election_data.expectedVoteVectorSize) return; //Got a vote for an expired election
        int origin = msg->getVehicleId();
        if(!received_votes[origin]){
            for(int i = 0; i < size; i++){
                databuffer->getVotesBuffer().push_back(msg->getVotes(i));
            }
            received_votes[origin] = true;
            bool allReceivedFlag = true;
            for(auto it = received_votes.begin(); it != received_votes.end(); ++it){
                allReceivedFlag &= it->second;
                if(!allReceivedFlag) break;
            }
            if(allReceivedFlag){
                sendUnicast(databuffer, targetLeaderId);
                regroupState = RegroupState::AWAITING_WINNER_DETERMINATION;
            }
        }
    }else{
        RouteVotingAppl::handleSubmitVote(msg);
    }
}

void RegroupAppl::handleEndOfVote(){
    Enter_Method_Silent();
    scheduleAt(simTime() + 1, regroupDelay);
}

void RegroupAppl::handleSelfMsg(cMessage* msg){
    if(msg == startInitialVote){
        /*Vote only speeds. No need to vote on routes. That is verified in scenario 2*/
        BeliefModel mnv("start/vote/speed"); //DEBUG STATE 2
        double args = 0;
        mnv.pushDouble(&args);
        manager->sendInformationToAgents(myId, &mnv);
        cycle = VoteCycle::SPEED_VOTE;
        negotiationState = VoteState::CHAIR_ELECTION_ONGOING;
    }else if(msg == regroupDelay){
        /*The first vote for speed has ended. The sender leader sends a request*/
        if((leaderRole == LeaderRole::SENDER) && (regroupState == RegroupState::NONE)){
            sendMemberExchange(RegroupMsgTypes::REQUEST); //DEBUG STATE 3
            regroupState = RegroupState::REQUESTING_EXCHANGE;
        }
        if((leaderRole == LeaderRole::RECEIVER) && (regroupState == RegroupState::AWAITING_ACK_ACCEPT)){
            //Have we already received a regroupMessage?
            sendMemberExchange(RegroupMsgTypes::OK); //DEBUG STATE ALT POST-3
            createRegroupElection(buffer);
            delete buffer;
            buffer = NULL;
        }
    }  else{
        RouteVotingAppl::handleSelfMsg(msg);
    }
}
