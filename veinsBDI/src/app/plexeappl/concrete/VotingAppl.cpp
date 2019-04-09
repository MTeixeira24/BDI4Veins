/*
 * VotingAppl.cpp
 *
 *  Created on: 12/09/2018
 *      Author: miguel
 */

#include "../concrete/VotingAppl.h"

Define_Module(VotingAppl);


VotingAppl::~VotingAppl() {
    //cancelAndDelete(awaitAckTimer);
    //if(searchTimer != NULL)
    //     cancelAndDelete(searchTimer);
    if(awaitAckTimer != NULL)
            cancelAndDelete(awaitAckTimer);
    if(voteTimer != NULL)
        cancelAndDelete(voteTimer);
}

void VotingAppl::initialize(int stage){
    GeneralPlexeAgentAppl::initialize(stage);
    if (stage == 1){
        awaitAckTimer = new cMessage("awaitAckTimer");
        // connect negotiation application to protocol
        protocol->registerApplication(NEGOTIATION_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"), gate("lowerControlIn"), gate("lowerControlOut"));
        // set initial beliefs
        //set speed from config file
        int desiredSpeed = ((VoteManager*)manager)->getPreferredSpeed(myId);
        Trigger ds(Belief("set/prefered/speed"), myId);
        ds.appendInt(desiredSpeed);
        manager->QueueTrigger(ds);
        if(getPlatoonRole() == PlatoonRole::NONE) {
            //This is the joiner vehicle. Call the connection manager to add belief to join platoon to agent
            if(par("engageNegotiations").boolValue()){
                int targetPlatoon = par("targetPlatoon").intValue();
                if(targetPlatoon >= 0){
                    Trigger bm(Belief("foundplatoon"), myId, targetPlatoon);
                    bm.appendInt(0);;
                    manager->QueueTrigger(bm);
                }else{
                    Trigger bm(Belief("lookforplatoon"), myId);
                    manager->QueueTrigger(bm);
                    searchingForPlatoon = true;
                    searchTimer = new cMessage("searchTimer");
                    //Search for platoons for half a second
                    scheduleAt(simTime() + SimTime(0.5), searchTimer);
                }
            }
        }else{
            //This a vehicle belonging to a platoon
            //Platoon beliefs
            Trigger pbelief(Belief("inplatoon"), myId, positionHelper->getPlatoonId());
            pbelief.appendInt(positionHelper->getLeaderId());
            manager->QueueTrigger(pbelief);
            //Beliefs about the current speed
            Trigger platoonSpeedBelief(Belief("set/speed"), myId, (int)(positionHelper->getPlatoonSpeed() * 3.6));
            manager->QueueTrigger(platoonSpeedBelief);
            if (getPlatoonRole() == PlatoonRole::LEADER) {
                voteTimer = new cMessage("VoteTimerA");
                //This is a leader, push the beliefs that allow an agent to know its role
                Trigger cbelief(Belief("ischair"), myId, positionHelper->getPlatoonId());
                manager->QueueTrigger(cbelief);
                std::vector<int> members = positionHelper->getPlatoonFormation();
                for (uint32_t i = 0; i < members.size(); i++){
                    Trigger mbelief(Belief("addmember"), myId, members[i]);
                    manager->QueueTrigger(mbelief);
                    //initiate vote list:
                    received_votes[members[i]] = false;
                }
            }
            else if (getPlatoonRole() == PlatoonRole::FOLLOWER){
                //This is a member
            }
        }
    }
}

void VotingAppl::setVote(std::vector<int> votes){
    vote = votes;
}

void VotingAppl::backupMessage(NegotiationMessage* msg){
    if(copy == NULL)
        delete copy;
    copy = msg->dup();
}

NotifyVote* VotingAppl::fillNotificationOfVote(int contextId, std::vector<double>& contextArgs, std::vector<int>& candidates){
    NotifyVote* msg = new NotifyVote("NotifyVote");
    msg->setVehicleId(myId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
    msg->setDestinationId(-1);
    msg->setKind(NEGOTIATION_TYPE);
    msg->setContextId(contextId);
    msg->setCandidatesArraySize(candidates.size());
    for(uint32_t i = 0; i < candidates.size(); i++){
        msg->setCandidates(i, candidates[i]);
    }
    if(contextArgs.empty()){
        msg->setContextArgumentsArraySize(0);
    }else{
        msg->setContextArgumentsArraySize(contextArgs.size());
        for(uint32_t i = 0; i < contextArgs.size(); i++){
            msg->setContextArguments(i, contextArgs[i]);
        }
    }
    return msg;
}

void VotingAppl::sendNotificationOfVoteDirect(VoteData electionData, int destinationId){
    Enter_Method_Silent();
    NotifyVote* msg = fillNotificationOfVote(electionData.contextId, electionData.contextArgs, electionData.candidates);
    msg->setDestinationId(destinationId);
    sendUnicast(msg, destinationId);
}

void VotingAppl::sendNotificationOfVoteGeneral(int contextId, std::vector<double>& contextArgs, std::vector<int>& candidates, int expectedVoteVector){
    Enter_Method_Silent();
    negotiationState = VoteState::CHAIR_ELECTION_ONGOING;
    NotifyVote* msg = fillNotificationOfVote(contextId, contextArgs, candidates);
    std::vector<int> members = positionHelper->getPlatoonFormation();
    //Set the the list of received votes
    for (uint32_t i = 0; i < members.size(); i++){
        received_votes[members[i]] = false;
    }
    //The leader agent stores its vote in the lightjason side
    received_votes[myId] = true;
    cancelEvent(voteTimer);
    scheduleAt(simTime() + 0.5, voteTimer);
    //Store the election data in case someone fails to receive it
    election_data.expectedVoteVectorSize = expectedVoteVector;
    election_data.candidates = candidates;
    election_data.contextId = contextId;
    election_data.contextArgs = contextArgs;
    sendUnicast(msg, -1);
}

void VotingAppl::sendRequestToJoin(int targetPlatooId, int destinationId, double preferedSpeed, double tolerance){
    Enter_Method_Silent();
    RequestJoinPlatoonMessage* msg = new RequestJoinPlatoonMessage("RequestJoinPlatoonMessage");
    targetLeaderId = destinationId;
    negotiationState = VoteState::JOINER_AWAITING_ACK_JOIN_REQUEST;
    msg->setKind(NEGOTIATION_TYPE);
    msg->setVehicleId(myId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
    msg->setPlatoonId(targetPlatooId);
    msg->setDestinationId(destinationId);
    msg->setPreferedSpeed(preferedSpeed);
    msg->setTolerance(tolerance);
    backupMessage(msg);
    sendUnicast(msg, destinationId);
    //Wait half a second to request ack if no response is heard
    cancelEvent(awaitAckTimer);
    scheduleAt(simTime() + ackTimeOut, awaitAckTimer);
}

void VotingAppl::sendVoteSubmition(std::vector<int>& votes){
    Enter_Method_Silent();
    SubmitVote* msg = new SubmitVote("SubmitVote");
    int leaderId = positionHelper->getLeaderId();
    msg->setKind(NEGOTIATION_TYPE);
    msg->setPlatoonId(positionHelper->getPlatoonId());
    msg->setVehicleId(myId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
    msg->setDestinationId(leaderId);
    msg->setVotesArraySize(votes.size());
    for(uint32_t i = 0; i < votes.size(); i++){
        msg->setVotes(i, votes[i]);
    }
    //Send the vote submission with a random delay between 10 and 50 ms to simulate processing time
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> distribution(25,2.0);
    double delay = distribution(gen) * 0.001;
    scheduleAt(simTime() + delay, msg);
    //Vote sent, wait for ack.
    negotiationState = VoteState::AWAITING_ACK_SUBMIT;
    cancelEvent(awaitAckTimer);
    //Reschedule to re-send 500ms from now if no ack is received
    scheduleAt(simTime() + ackTimeOut, awaitAckTimer);
}

void VotingAppl::sendVoteResults(int winnerValue, int joinerId){
    negotiationState = VoteState::CHAIR_ELECTION_END;
    NotifyResults* msg = new NotifyResults("NotifyResults");
    //Save results for future use
    election_data.currentResult = winnerValue;
    election_data.joinerId = joinerId;
    int platoonId = positionHelper->getPlatoonId();
    msg->setKind(NEGOTIATION_TYPE);
    msg->setVehicleId(myId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
    msg->setDestinationId(-1);
    msg->setResult(winnerValue);
    msg->setJoinerId(joinerId);
    msg->setPlatoonId(platoonId);
    std::cout << "SENT RESULTS" << std::endl; //GOT VOTE RESULT
    sendUnicast(msg, -1);
}

void VotingAppl::sendCommitteeVoteResults(std::vector<int>& results){
    throw cRuntimeError("sendCommitteeVoteResults, not implemented!");
}

void VotingAppl::onPlatoonBeacon(const PlatooningBeacon* pb){
    GeneralPlexeAgentAppl::onPlatoonBeacon(pb);
}

void VotingAppl::sendAck(AckType ack_type, int destination){
    Ack* ack = new Ack("Ack");
    ack->setAckType((int)ack_type);
    ack->setDestinationId(destination);
    ack->setKind(NEGOTIATION_TYPE);
    ack->setVehicleId(myId);
    ack->setExternalId(positionHelper->getExternalId().c_str());
    sendUnicast(ack, destination);
}

void VotingAppl::sendResultRequest(int originId, int targetId){
    Enter_Method_Silent();
    RequestResults* msg = new RequestResults("RequestResults");
    msg->setPlatoonId(positionHelper->getPlatoonId());
    msg->setDestinationId(targetId);
    msg->setKind(NEGOTIATION_TYPE);
    msg->setVehicleId(myId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
    sendUnicast(msg, targetId);
    cancelEvent(awaitAckTimer);
    //Reschedule to re-send 500ms from now if no ack is received
    scheduleAt(simTime() + ackTimeOut, awaitAckTimer);
}

void VotingAppl::fillNegotiationMessage(NegotiationMessage* msg, int originId, int targetId){
    msg->setDestinationId(targetId);
    msg->setKind(NEGOTIATION_TYPE);
    msg->setVehicleId(originId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
}

void VotingAppl::handleLowerMsg(cMessage* msg){
    UnicastMessage* unicast = check_and_cast<UnicastMessage*>(msg);

    cPacket* enc = unicast->getEncapsulatedPacket();
    ASSERT2(enc, "received a UnicastMessage with nothing inside");

    if (enc->getKind() == NEGOTIATION_TYPE) {
        NegotiationMessage* nm = check_and_cast<NegotiationMessage*>(unicast->decapsulate());
        delegateNegotiationMessage(nm);
        delete unicast;
    }
    else {
        GeneralPlexeAgentAppl::handleLowerMsg(msg);
    }
}

void VotingAppl::delegateNegotiationMessage(NegotiationMessage* nm){
    if (RequestJoinPlatoonMessage* msg = dynamic_cast<RequestJoinPlatoonMessage*>(nm)) {
        handleRequestToJoinNegotiation(msg);
        delete msg;
    }else if (SubmitVote* msg = dynamic_cast<SubmitVote*>(nm)) {
        handleSubmitVote(msg);
        delete msg;
    }else if (NotifyResults* msg = dynamic_cast<NotifyResults*>(nm)) {
        handleNotificationOfResults(msg);
        delete msg;
    }else if (NotifyVote* msg = dynamic_cast<NotifyVote*>(nm)) {
        if((msg->getDestinationId() == -1) || (msg->getDestinationId() == myId)){
            handleNotifyVote(msg);
        }
        delete msg;
    }else if(RequestResults* msg = dynamic_cast<RequestResults*>(nm)) {
        handleRequestResults(msg);
        delete msg;
    } else if (Ack* msg = dynamic_cast<Ack*>(nm)) {
        handleAck(msg);
        delete msg;
    }
}

void VotingAppl::handleRequestResults(RequestResults* rr){
    switch(negotiationState){
    case VoteState::CHAIR_ELECTION_ONGOING:{
        //We dont want to send old election data back
        //Until the agent gives us a new iteration or the results back
        //ignore requests for results
        if(!received_votes[rr->getVehicleId()])
            sendNotificationOfVoteDirect(election_data, rr->getVehicleId());
        break;
    }
    case VoteState::CHAIR_SEARCHING_JOINERS:
    case VoteState::CHAIR_ELECTION_END:{
        NotifyResults* msg = new NotifyResults("NotifyResults");
        int platoonId = positionHelper->getPlatoonId();
        msg->setKind(NEGOTIATION_TYPE);
        msg->setVehicleId(myId);
        msg->setExternalId(positionHelper->getExternalId().c_str());
        msg->setDestinationId(rr->getVehicleId());
        msg->setResult(election_data.currentResult);
        msg->setJoinerId(election_data.joinerId);
        msg->setPlatoonId(platoonId);
        sendUnicast(msg, rr->getVehicleId());
        break;
    }
    default:{
        break;
    }
    }
}

void VotingAppl::handleRequestToJoinNegotiation(const RequestJoinPlatoonMessage* msg){
    //Push the knowledge of a potential joiner to the agent
    int joinerId = msg->getVehicleId();
    double joinerSpeed = msg->getPreferedSpeed();
    double joinerPreference = msg->getTolerance();
    Trigger jbelief(Belief("requestjoin"), myId,joinerId);
    jbelief.appendInt(joinerSpeed);
    jbelief.appendInt(joinerPreference);
//    jbelief.setBelief("requestjoin");
//    jbelief.pushInt(&joinerId);
//    jbelief.pushDouble(&joinerSpeed);
//    jbelief.pushDouble(&joinerPreference);
//    manager->sendInformationToAgents(myId, &jbelief);
    manager->QueueTrigger(jbelief);
}

void VotingAppl::handleSubmitVote(const SubmitVote* msg){
    if(msg->getPlatoonId() != positionHelper->getPlatoonId()) return;
    if(myId != positionHelper->getLeaderId()) return;
    int size = msg->getVotesArraySize();
    if(size > election_data.expectedVoteVectorSize) return; //Got a vote for an expired election
    int origin = msg->getVehicleId();
    if(!received_votes[origin]){
        std::vector<int> votes(size);
        for(int i = 0; i < size; i++){
            votes[i] = msg->getVotes(i);
        }
        Trigger voteSubmission(Belief("handle/submit/vote"), myId);
        voteSubmission.appendVector(votes);
        voteSubmission.appendInt(origin);
        manager->QueueTrigger(voteSubmission);
        received_votes[origin] = true;
    }
    //Got the vote. Notify of successful delivery
    sendAck(AckType::VOTE_RECEIVED, origin);
}

void VotingAppl::handleNotificationOfResults(const NotifyResults* msg){
    if(msg->getJoinerId() != -1){
        std::cout << "@@@@@@@@@@@ GOT VOTE RESULT " << myId << "@@@@@@@@@@@ " << msg->getResult() << std::endl;
        negotiationState = VoteState::NONE;
        if( (positionHelper->getPlatoonId()) == (msg->getPlatoonId()) ){
            cancelEvent(awaitAckTimer);
            //TODO: Handle insertion of beliefs
        }else if (myId == msg->getJoinerId()){
            cancelEvent(awaitAckTimer);
            if(msg->getResult() == 1)
                startJoinManeuver(msg->getPlatoonId(), msg->getVehicleId(), -1);
            else{
                Trigger result(Belief("handlerejection"), myId, msg->getPlatoonId());
                manager->QueueTrigger(result);
            }
        }
    }else{
        if(positionHelper->getPlatoonId() != msg->getPlatoonId()) return;
        if(negotiationState != VoteState::AWAITING_RESULTS) return;
        cancelEvent(awaitAckTimer);
        std::cout << "############ GOT VOTE RESULT " << myId << "############## " << msg->getResult() << std::endl;
        Trigger result(Belief("handle/results"), myId, msg->getResult());
        manager->QueueTrigger(result);
        negotiationState = VoteState::NONE;
    }
}

void VotingAppl::handleAck(const Ack* msg){
    AckType type = (AckType)(msg->getAckType());
    if(type == AckType::VOTE_RECEIVED){
        negotiationState = VoteState::AWAITING_RESULTS;
        //Wait a around 500ms for the results, if there are none send a request for results
        std::random_device rd{};
        std::mt19937 gen{rd()};
        std::normal_distribution<double> distribution(50,2.0);
        double delay = distribution(gen) * 0.01;
        cancelEvent(awaitAckTimer);
        scheduleAt(simTime() + ackTimeOut + delay, awaitAckTimer);
    }
}

void VotingAppl::handleSelfMsg(cMessage* msg){
    if (SubmitVote* sv = dynamic_cast<SubmitVote*>(msg)){
        sendUnicast(sv, sv->getDestinationId());
        //delete msg;
    }else if(msg == searchTimer){
        Trigger sendRequests(Belief("startrequests"), myId);
        manager->QueueTrigger(sendRequests);
        searchingForPlatoon = false;
        delete msg;
        searchTimer = NULL;
    }else if (msg == awaitAckTimer){
        if(negotiationState == VoteState::AWAITING_ACK_SUBMIT){
            sendVoteSubmition(vote);
            ((VoteManager*)manager)->incrementRetransmission();
        } else if (negotiationState == VoteState::AWAITING_RESULTS){
            //The joiner has not yet received results
            if(targetLeaderId > -1)
                sendResultRequest(myId, targetLeaderId);
            else //The members have not yet received the results
                sendResultRequest(myId, positionHelper->getLeaderId());
        }
    }else if(msg == voteTimer){
        //Count how many absentees there are
        int absentees = 0;
        for( const auto& kv_pair : received_votes ){
            if(!kv_pair.second){
                absentees++;
                //Resend the notification of vote to whoever is missing
                sendNotificationOfVoteDirect(election_data, kv_pair.first);
                ((VoteManager*)manager)->incrementRetransmission();
            }
        }
        if(absentees > 0){
            scheduleAt(simTime() + 0.5, voteTimer);
        }
    }else {
        GeneralPlexeAgentAppl::handleSelfMsg(msg);
    }
}

void VotingAppl::handleNotifyVote(const NotifyVote* msg){
    if (positionHelper->isInSamePlatoon(msg->getVehicleId())) { // Verify that it is from this platoon
        negotiationState = VoteState::NONE;
        Trigger voteNotify(Belief("handle/vote/notification"), myId);
        std::vector<int> contextArgs;
        uint32_t size = msg->getCandidatesArraySize();
        std::vector<int> candidates(size);
        for(uint32_t i = 0; i < size; i++){
            candidates[i] = msg->getCandidates(i);
        }
        voteNotify.appendVector(candidates);
        fillContextVector(msg, contextArgs);
        voteNotify.appendVector(contextArgs);
        manager->QueueTrigger(voteNotify);
    }
}

void VotingAppl::fillContextVector(const NotifyVote* msg, std::vector<int>& contextArgs){
    contextArgs.push_back(msg->getContextId());
    if(msg->getContextId() == CONTEXT_JOIN)
    {
        for(uint32_t i = 1; i < msg->getContextArgumentsArraySize() + 1; i++){
            contextArgs[i] = msg->getContextArguments(i - 1);
        }
    }
    for(uint32_t i = 0; i < msg->getContextArgumentsArraySize(); i++)
        contextArgs.push_back(msg->getContextArguments(i));
}

void VotingAppl::sendToAgent(const BeliefModel* bm){
    manager->sendInformationToAgents(myId, bm);
}
