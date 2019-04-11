/*
 * BaseVoteAppl.cpp
 *
 *  Created on: Apr 5, 2019
 *      Author: miguel
 */

#include "BaseVoteAppl.h"

Define_Module(BaseVoteAppl);

BaseVoteAppl::BaseVoteAppl() {
    voteTimer = NULL;
    negotiationState = VoteState::NONE;
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
        voteManager = (VoteAgentManager*)manager;
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
    negotiationState = VoteState::SPEED;
    voteTimer = new cMessage("VoteTimerInitial");
    std::vector<int> members = positionHelper->getPlatoonFormation();
    voteMembers.insert(members.begin() + 1, members.end());
    scheduleAt(simTime() + 1, voteTimer);
}

void BaseVoteAppl::handleEndOfVote(){
    if(negotiationState == VoteState::SPEED){
        negotiationState = VoteState::ROUTE;
        voteTimer = new cMessage("VoteTimerRoute");
        scheduleAt(simTime() + 1, voteTimer);
    }
}


void BaseVoteAppl::fillNegotiationMessage(NegotiationMessage* msg, int originId,
        int targetId, int idOfOriginMessage, bool forWholePlatoon){
    long msgId = rand();
    msg->setForWholePlatoon(forWholePlatoon);
    msg->setMessageId(msgId);
    msg->setDestinationId(targetId);
    msg->setKind(NEGOTIATION_TYPE);
    msg->setVehicleId(originId);
    msg->setReplyMessageId(idOfOriginMessage);
    msg->setPlatoonId(positionHelper->getPlatoonId());
    msg->setExternalId(positionHelper->getExternalId().c_str());
}
void BaseVoteAppl::fillNegotiationMessage(NegotiationMessage* msg, int originId,
        int targetId, std::unordered_set<int>& targets, int idOfOriginMessage,
        bool forWholePlatoon){
    fillNegotiationMessage(msg, originId, targetId, idOfOriginMessage, forWholePlatoon);
    msg->setTargets(targets);
}

void BaseVoteAppl::sendVoteSubmition(std::vector<int>& votes){
    Enter_Method_Silent();
    vote = votes;
    SubmitVote* msg = new SubmitVote("SubmitVote");
    int leaderId = positionHelper->getLeaderId();
    fillNegotiationMessage(msg, myId, leaderId);
    msg->setVotesArraySize(votes.size());
    for(uint32_t i = 0; i < votes.size(); i++){
        msg->setVotes(i, votes[i]);
    }
    sendMessageDelayed(msg, leaderId);
}

void BaseVoteAppl::sendVoteResults(int winnerValue, int joinerId){
    Enter_Method_Silent();
    NotifyResults* msg = new NotifyResults("NotifyResults");
    //Save results for future use
    election_data.currentResult = winnerValue;
    election_data.joinerId = joinerId;
    fillNegotiationMessage(msg, myId, -1, voteMembers);
    msg->setResult(winnerValue);
    msg->setJoinerId(joinerId);
    std::vector<int> members(positionHelper->getPlatoonFormation().begin() + 1,
            positionHelper->getPlatoonFormation().end());
    sendMessageWithAck(msg, members);
    handleEndOfVote();
}

void BaseVoteAppl::sendCommitteeVoteResults(std::vector<int>& results){
    Enter_Method_Silent();
    NotifyResults* msg = new NotifyResults("NotifyCommitteeResults");
    //Save results for future use
    election_data.currentResult = -1;
    election_data.committeeResult = results;
    fillNegotiationMessage(msg, myId, -1, voteMembers);
    msg->setResult(-1);
    msg->setCommitteeResultArraySize(results.size());
    for(uint32_t i = 0; i < results.size(); i++) msg->setCommitteeResult(i, results[i]);
    std::vector<int> members(positionHelper->getPlatoonFormation().begin() + 1,
            positionHelper->getPlatoonFormation().end());
    sendMessageWithAck(msg, members);
    handleEndOfVote();
    //Set a delay to start the speed vote
    /*if(startSpeedVoteDelay == NULL)
        startSpeedVoteDelay = new cMessage("startSpeedVoteDelay");
    cancelEvent(startSpeedVoteDelay);
    scheduleAt(simTime() + 0.1, startSpeedVoteDelay);*/
}

void BaseVoteAppl::sendNotificationOfVoteGeneral(int contextId, std::vector<double>& contextArgs, std::vector<int>& candidates, int expectedVoteVector){
    Enter_Method_Silent();
    NotifyVote* msg = fillNotificationOfVote(contextId, contextArgs, candidates);
    //Set the the list of received votes
    std::vector<int> members(positionHelper->getPlatoonFormation().begin() + 1,
            positionHelper->getPlatoonFormation().end());
    for (uint32_t i = 0; i < members.size(); i++){
        received_votes[members[i]] = false;
    }
    //cancelEvent(voteTimer);
    //scheduleAt(simTime() + 0.5, voteTimer);
    //Store the election data in case someone fails to receive it
    election_data.expectedVoteVectorSize = expectedVoteVector;
    election_data.candidates = candidates;
    election_data.contextId = contextId;
    election_data.contextArgs = contextArgs;
    sendMessageWithAck(msg, members);
    //sendUnicast(msg, -1);
}

NotifyVote* BaseVoteAppl::fillNotificationOfVote(int contextId, std::vector<double>& contextArgs, std::vector<int>& candidates){
    NotifyVote* msg = new NotifyVote("NotifyVote");
    fillNegotiationMessage(msg, myId, -1, voteMembers);
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
           voteTimer = NULL;
           if(negotiationState == VoteState::SPEED){
               Trigger trig(Belief("handle/vote/start"), myId, CONTEXT_SPEED);
               std::vector<int> context(1,0);
               trig.appendVector(context);
               manager->QueueTrigger(trig);
           }else if(negotiationState == VoteState::ROUTE){
               Trigger trig(Belief("handle/vote/start"), myId, CONTEXT_PATH);
               std::vector<int> context(1,0);
               trig.appendVector(context);
               manager->QueueTrigger(trig);
           }
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
    }else*/ if (SubmitVote* msg = dynamic_cast<SubmitVote*>(nm)) {
        if(isReceiver(msg)) handleSubmitVote(msg);
        delete msg;
    }else if (NotifyResults* msg = dynamic_cast<NotifyResults*>(nm)) {
        if(isReceiver(msg)) handleNotificationOfResults(msg);
        delete msg;
    }else if (NotifyVote* msg = dynamic_cast<NotifyVote*>(nm)) {
        if((msg->getDestinationId() == -1) || (msg->getDestinationId() == myId)){
            if(isReceiver(msg)) handleNotifyVote(msg);
        }
        delete msg;
    }/*else if(RequestResults* msg = dynamic_cast<RequestResults*>(nm)) {
        handleRequestResults(msg);
        delete msg;
    }*/
}

void BaseVoteAppl::handleSubmitVote(const SubmitVote* msg){
    if(!msg->getIsAck()){
        if(!messageCache.hasReceived(msg->getMessageId())){
            messageCache.saveReceived(msg->getMessageId());
            if(msg->getPlatoonId() != positionHelper->getPlatoonId()) return;
            if(myId != positionHelper->getLeaderId()) return;
            int size = msg->getVotesArraySize();
            //Got a vote for an expired election
            if(size > election_data.expectedVoteVectorSize) return;
            int origin = msg->getVehicleId();
            if(!received_votes[origin]){
                std::vector<int> votes(size);
                for(int i = 0; i < size; i++){
                    votes[i] = msg->getVotes(i);
                }
                Trigger voteSubmission(Belief("handle/vote/submit"), myId);
                voteSubmission.appendVector(votes);
                voteSubmission.appendInt(origin);
                manager->QueueTrigger(voteSubmission);
                received_votes[origin] = true;
            }
        }
        SubmitVote* reply = new SubmitVote("SubmitAck");
        reply->setIsAck(true);
        fillNegotiationMessage(reply, myId, msg->getVehicleId(), msg->getMessageId());
        sendMessageDelayed(reply, msg->getVehicleId());
    }else{
        messageCache.markReceived(msg->getReplyMessageId(), msg->getVehicleId());
    }
    //Got the vote. Notify of successful delivery
    //sendAck(AckType::VOTE_RECEIVED, origin);
}


void BaseVoteAppl::handleNotifyVote(const NotifyVote* msg){
    if(!msg->getIsAck()){
        if(!messageCache.hasReceived(msg->getMessageId())){
            Trigger voteNotify(Belief("handle/vote/notification"), myId);
            std::vector<int> contextArgs(1 + msg->getContextArgumentsArraySize());
            uint32_t size = msg->getCandidatesArraySize();
            std::vector<int> candidates(size);
            for(uint32_t i = 0; i < size; i++){
                candidates[i] = msg->getCandidates(i);
            }
            voteNotify.appendVector(candidates);


            contextArgs[0] = msg->getContextId();
            if(msg->getContextId() == CONTEXT_JOIN)
            {
                for(uint32_t i = 1; i < msg->getContextArgumentsArraySize() + 1; i++){
                    contextArgs[i] = msg->getContextArguments(i - 1);
                }
            }
            for(uint32_t i = 0; i < msg->getContextArgumentsArraySize(); i++)
                contextArgs.push_back(msg->getContextArguments(i));


            voteNotify.appendVector(contextArgs);
            manager->QueueTrigger(voteNotify);
        }
        NotifyVote* reply = new NotifyVote("NotifyVoteAck");
        reply->setIsAck(true);
        fillNegotiationMessage(reply, myId, msg->getVehicleId(), msg->getMessageId());
        sendMessageDelayed(reply, msg->getVehicleId());
    }else{
        messageCache.markReceived(msg->getReplyMessageId(), msg->getVehicleId());
    }

}

void BaseVoteAppl::handleNotificationOfResults(const NotifyResults* msg){
    if(!msg->getIsAck()){
        if(!messageCache.hasReceived(msg->getMessageId())){
            if(msg->getResult() == -1){
                std::vector<int> resultsVector(msg->getCommitteeResultArraySize());
                for(uint32_t i = 0; i < msg->getCommitteeResultArraySize(); i++) resultsVector[i] = msg->getCommitteeResult(i);
                Trigger result(Belief("handle/results"), myId, resultsVector);
                result.appendInt(CONTEXT_PATH);
                manager->QueueTrigger(result);
            }else{
                Trigger result(Belief("handle/results"), myId, msg->getResult());
                result.appendInt(CONTEXT_SPEED);
                manager->QueueTrigger(result);
            }
        }
        NotifyResults* reply = new NotifyResults("NotifyResultsAck");
        reply->setIsAck(true);
        fillNegotiationMessage(reply, myId, msg->getVehicleId(), msg->getMessageId());
        sendMessageDelayed(reply, msg->getVehicleId());
    }else{
        messageCache.markReceived(msg->getReplyMessageId(), msg->getVehicleId());
    }
}

void BaseVoteAppl::sendMessageWithAck(NegotiationMessage* msg, int target){
    std::vector<int> targets({target});
    sendMessageWithAck(msg, targets);
}

void BaseVoteAppl::sendMessageWithAck(NegotiationMessage* msg, const std::vector<int>& targets){
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

bool BaseVoteAppl::isReceiver(NegotiationMessage* msg){
    if(msg->getDestinationId() == myId)
        return true;
    if(msg->getTargets().find(myId) != msg->getTargets().end())
        return true;
    return false;
}

void BaseVoteAppl::sendMessageDelayed(NegotiationMessage* msg, int target){
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
