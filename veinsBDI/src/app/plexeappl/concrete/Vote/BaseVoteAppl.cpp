/*
 * BaseVoteAppl.cpp
 *
 *  Created on: Apr 5, 2019
 *      Author: miguel
 */

#include "BaseVoteAppl.h"

BaseVoteAppl::BaseVoteAppl() {
    voteManager = (VoteManager*)manager;
    voteTimer = NULL;
}

BaseVoteAppl::~BaseVoteAppl() {
    if(voteTimer != NULL)
        cancelAndDelete(voteTimer);
}
void BaseVoteAppl::initialize(int stage){
    GeneralPlexeAgentAppl::initialize(stage);
    if(stage == 2){
        messageCache.setSenderId(myId);
        protocol->registerApplication(NEGOTIATION_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"),
                        gate("lowerControlIn"), gate("lowerControlOut"));
        traciVehicle->setSpeed(mobility->getSpeed());
        traciVehicle->setFixedLane(traciVehicle->getLaneIndex(), false);
    }
}

void BaseVoteAppl::setInitialBeliefs(){
    int isLeader = getPlatoonRole() == PlatoonRole::LEADER;
    Trigger beliefs(Belief("setup/beliefs"), myId, isLeader);
    beliefs.appendInt((int)(positionHelper->getPlatoonSpeed() * 3.6));
    beliefs.appendInt(voteManager->getPreferredSpeed(myId));
    beliefs.appendVector(voteManager->getPreferredPath(myId));
    beliefs.appendVector(positionHelper->getPlatoonFormation());
    if(isLeader){
        initialLeaderBehaviour();
    }
    manager->QueueTrigger(beliefs);
}

void BaseVoteAppl::initialLeaderBehaviour(){

    voteTimer = new cMessage("VoteTimerInitial");
    std::vector<int> members = positionHelper->getPlatoonFormation();
    scheduleAt(simTime() + 0.5, voteTimer);
    for (uint32_t i = 0; i < members.size(); i++){
        received_votes[members[i]] = false;
    }
}

void BaseVoteAppl::sendVoteSubmition(std::vector<int>& votes){
    Enter_Method_Silent();
    vote = votes;
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
    /*std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> distribution(25,2.0);
    double delay = distribution(gen) * 0.001;
    scheduleAt(simTime() + delay, msg);
    //Vote sent, wait for ack.
    negotiationState = VoteState::AWAITING_ACK_SUBMIT;
    cancelEvent(awaitAckTimer);
    //Reschedule to re-send 500ms from now if no ack is received
    scheduleAt(simTime() + ackTimeOut, awaitAckTimer);*/
}

void BaseVoteAppl::sendVoteResults(int winnerValue, int joinerId){
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
    /*std::cout << "SENT RESULTS" << std::endl; //GOT VOTE RESULT
    sendUnicast(msg, -1);*/
}

void BaseVoteAppl::sendCommitteeVoteResults(std::vector<int>& results){
    Enter_Method_Silent();
    negotiationState = VoteState::CHAIR_ELECTION_END;
    NotifyResults* msg = new NotifyResults("NotifyResults");
    //Save results for future use
    election_data.currentResult = -1;
    election_data.committeeResult = results;
    int platoonId = positionHelper->getPlatoonId();
    msg->setKind(NEGOTIATION_TYPE);
    msg->setVehicleId(myId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
    msg->setDestinationId(-1);
    msg->setResult(-1);
    msg->setPlatoonId(platoonId);
    msg->setCommitteeResultArraySize(results.size());
    for(uint32_t i = 0; i < results.size(); i++) msg->setCommitteeResult(i, results[i]);
    /*sendUnicast(msg, -1);
    //Set a delay to start the speed vote
    if(startSpeedVoteDelay == NULL)
        startSpeedVoteDelay = new cMessage("startSpeedVoteDelay");
    cancelEvent(startSpeedVoteDelay);
    scheduleAt(simTime() + 0.1, startSpeedVoteDelay);*/
}

void BaseVoteAppl::sendNotificationOfVoteGeneral(int contextId, std::vector<double>& contextArgs, std::vector<int>& candidates, int expectedVoteVector){
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
    //sendUnicast(msg, -1);
}

NotifyVote* BaseVoteAppl::fillNotificationOfVote(int contextId, std::vector<double>& contextArgs, std::vector<int>& candidates){
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

void BaseVoteAppl::handleLowerMsg(cMessage* msg){
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

void BaseVoteAppl::handleSelfMsg(cMessage* msg){
    if (AckTimer* at = dynamic_cast<AckTimer*>(msg)){
       long msgId = at->getMessageId();
           if(messageCache.allResponded(msgId)){
               delete at;
           }else{
               resendMessage(msgId, at);
           }
       }else if(msg == voteTimer){
           delete msg;
           //traciVehicle->setFixedLane(traciVehicle->getLaneIndex() + 1, false);
           //Trigger beliefs(Belief("bargain/start"), myId, myId + 1);
           //manager->QueueTrigger(beliefs);
       }else if(NegotiationMessage* delayMessage = dynamic_cast<NegotiationMessage*>(msg)){
          sendMessageWithAck(delayMessage, delayMessage->getDestinationId());
       } else{
           GeneralPlexeAgentAppl::handleSelfMsg(msg);
       }
}

void BaseVoteAppl::delegateNegotiationMessage(NegotiationMessage* nm){
    /*if (RequestJoinPlatoonMessage* msg = dynamic_cast<RequestJoinPlatoonMessage*>(nm)) {
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
    }*/
}


void BaseVoteAppl::sendMessageWithAck(NegotiationMessage* msg, int target){
    std::vector<int> targets({target});
    messageCache.insertEntry(msg->getMessageId(), msg->dup(), targets);
    AckTimer* at = new AckTimer("AckTimer");
    at->setMessageId(msg->getMessageId());
    scheduleAt(simTime() + ackTime + randomOffset(), at);
    sendUnicast(msg, -1);
}

double BaseVoteAppl::randomOffset(){
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> distribution(25,5.0);
    return std::abs(distribution(gen) * 0.001);
}

bool BaseVoteAppl::isReceiver(MarketMessage* msg){
    if(msg->getDestinationId() == myId)
        return true;
    if(msg->getTargets().find(myId) != msg->getTargets().end())
        return true;
    return false;
}

void BaseVoteAppl::sendMessageDelayed(MarketMessage* msg, int target){
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> distribution(25,5.0);
    double delay = std::abs(distribution(gen) * 0.001);
    scheduleAt(simTime() + delay, msg);
}

void BaseVoteAppl::resendMessage(long msgId, AckTimer* at){
    NegotiationMessage* resend = messageCache.getMessageReference(msgId);
    resend->setForWholePlatoon(false);
    resend->setTargets(messageCache.getRemainerIds(msgId));
    scheduleAt(simTime() + ackTime + randomOffset(), at);
    sendUnicast(resend->dup(), -1);
}
