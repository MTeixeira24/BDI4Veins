/*
 * VotingAppl.cpp
 *
 *  Created on: 12/09/2018
 *      Author: miguel
 */

#include "VotingAppl.h"

Define_Module(VotingAppl);


VotingAppl::~VotingAppl() {
    //if(searchTimer != NULL)
    //     cancelAndDelete(searchTimer);
    //if(awaitAckTimer != NULL)
    //         cancelAndDelete(awaitAckTimer);
   // if(voteTimer != NULL)
   //          cancelAndDelete(voteTimer);
}

void VotingAppl::initialize(int stage){
    GeneralPlexeAgentAppl::initialize(stage);
    if (stage == 1){
        // connect negotiation application to protocol
        protocol->registerApplication(NEGOTIATION_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"), gate("lowerControlIn"), gate("lowerControlOut"));
        // set initial beliefs
        int desiredSpeed = (int)par("desiredSpeed").doubleValue();
        BeliefModel ds("set/prefered/speed");
        ds.pushInt(&desiredSpeed);
        manager->sendInformationToAgents(myId, &ds);
        if(getPlatoonRole() == PlatoonRole::NONE) {
            //This is the joiner vehicle. Call the connection manager to add belief to join platoon to agent
            BeliefModel bm;
            int targetPlatoon = par("targetPlatoon").intValue();
            if(targetPlatoon >= 0){
                bm.setBelief("foundplatoon");
                int platoonId = targetPlatoon;
                int leaderId = 0;
                bm.pushInt(&platoonId);
                bm.pushInt(&leaderId);
                manager->sendInformationToAgents(myId,&bm);
            }else{
                bm.setBelief("lookforplatoon");
                manager->sendInformationToAgents(myId,&bm);
                searchingForPlatoon = true;
                searchTimer = new cMessage();
                //Search for platoons for half a second
                scheduleAt(simTime() + SimTime(0.5), searchTimer);
            }
        }else{
            //This a vehicle belonging to a platoon
            if (getPlatoonRole() == PlatoonRole::LEADER) {
                //This is a leader, push the beliefs that allow an agent to know its role
                BeliefModel cbelief;
                BeliefModel pbelief;
                cbelief.setBelief("ischair");
                int platoonId = positionHelper->getPlatoonId();
                cbelief.pushInt(&platoonId);
                manager->sendInformationToAgents(myId, &cbelief);
                pbelief.setBelief("inplatoon");
                int leaderId = positionHelper->getLeaderId();
                pbelief.pushInt(&platoonId);
                pbelief.pushInt(&leaderId);
                manager->sendInformationToAgents(myId, &pbelief);
                BeliefModel platoonSpeedBelief("set/speed");
                int platoonSpeed = (positionHelper->getPlatoonSpeed() * 3.6);
                platoonSpeedBelief.pushInt(&platoonSpeed);
                manager->sendInformationToAgents(myId, &platoonSpeedBelief);
                std::vector<int> members = positionHelper->getPlatoonFormation();
                for (uint32_t i = 0; i < members.size(); i++){
                    BeliefModel mbelief;
                    mbelief.setBelief("addmember");
                    int member = members[i];
                    mbelief.pushInt(&member);
                    manager->sendInformationToAgents(myId, &mbelief);
                    //initiate vote list:
                    received_votes[member] = false;
                }
            }
            else if (getPlatoonRole() == PlatoonRole::FOLLOWER){
                //This is a member, push beliefs
                BeliefModel pbelief;
                pbelief.setBelief("inplatoon");
                int platoonId = positionHelper->getPlatoonId();
                int leaderId = positionHelper->getLeaderId();
                pbelief.pushInt(&platoonId);
                pbelief.pushInt(&leaderId);
                manager->sendInformationToAgents(myId, &pbelief);
                BeliefModel platoonSpeedBelief("set/speed");
                int platoonSpeed = (positionHelper->getPlatoonSpeed() * 3.6);
                platoonSpeedBelief.pushInt(&platoonSpeed);
                manager->sendInformationToAgents(myId, &platoonSpeedBelief);
            }
        }
    }
}

void VotingAppl::setVote(std::vector<int> votes){
    vote = votes;
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

void VotingAppl::sendNotificationOfVoteGeneral(int contextId, std::vector<double>& contextArgs, std::vector<int>& candidates){
    Enter_Method_Silent();
    NotifyVote* msg = fillNotificationOfVote(contextId, contextArgs, candidates);
    if(contextId == CONTEXT_SPEED) ((VoteManager*)manager)->storeTimeStamp(simTime().dbl() * 1000, VoteManager::TimeStampAction::TIME_OF_VOTE_START);
    std::vector<int> members = positionHelper->getPlatoonFormation();
    //Set the the list of received votes
    for (uint32_t i = 0; i < members.size(); i++){
        received_votes[members[i]] = false;
    }
    //The leader agent stores its vote in the lightjason side
    received_votes[myId] = true;
    voteTimer = new cMessage("VoteTimer");
    scheduleAt(simTime() + 0.5, voteTimer);
    //Store the election data in case someone fails to receive it
    election_data.candidates = candidates;
    election_data.contextId = contextId;
    election_data.contextArgs = contextArgs;
    sendUnicast(msg, -1);
}

void VotingAppl::sendRequestToJoin(int targetPlatooId, int destinationId, double preferedSpeed, double tolerance){
    RequestJoinPlatoonMessage* msg = new RequestJoinPlatoonMessage("RequestJoinPlatoonMessage");
    msg->setKind(NEGOTIATION_TYPE);
    msg->setVehicleId(myId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
    msg->setPlatoonId(targetPlatooId);
    msg->setDestinationId(destinationId);
    msg->setPreferedSpeed(preferedSpeed);
    msg->setTolerance(tolerance);
    sendUnicast(msg, destinationId);
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
    int position = positionHelper->getPosition();
    //Alterar para tempo aleatório
    //Ou usar sleep
    scheduleAt(simTime() + 0.1*position, msg);
    //Vote sent, wait for ack.
    negotiationState = VoteState::AWAITING_ACK_SUBMIT;
    awaitAckTimer = new cMessage("awaitAckTimer");
    //Reschedule to re-send 500ms from now if no ack is received
    scheduleAt(simTime() + 1, awaitAckTimer);
}

void VotingAppl::sendVoteResults(int winnerValue, int joinerId){
    NotifyResults* msg = new NotifyResults("NotifyResults");
    int platoonId = positionHelper->getPlatoonId();
    msg->setKind(NEGOTIATION_TYPE);
    msg->setVehicleId(myId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
    msg->setDestinationId(-1);
    msg->setResult(winnerValue);
    msg->setJoinerId(joinerId);
    msg->setPlatoonId(platoonId);
    ((VoteManager*)manager)->storeTimeStamp(simTime().dbl() * 1000, VoteManager::TimeStampAction::CHAIR_TO_JOINER_START);
    sendUnicast(msg, -1);
}

void VotingAppl::onPlatoonBeacon(const PlatooningBeacon* pb){
    GeneralPlexeAgentAppl::onPlatoonBeacon(pb);
}

void VotingAppl::sendAck(std::string ack_type, int destination){
    Ack* ack = new Ack("Ack");
    ack->setAckType(ack_type.c_str());
    ack->setDestinationId(destination);
    ack->setKind(NEGOTIATION_TYPE);
    ack->setVehicleId(myId);
    ack->setExternalId(positionHelper->getExternalId().c_str());
    sendUnicast(ack, destination);
}

void VotingAppl::handleLowerMsg(cMessage* msg){
    UnicastMessage* unicast = check_and_cast<UnicastMessage*>(msg);

    cPacket* enc = unicast->getEncapsulatedPacket();
    ASSERT2(enc, "received a UnicastMessage with nothing inside");

    if (enc->getKind() == NEGOTIATION_TYPE) {
        NegotiationMessage* nm = check_and_cast<NegotiationMessage*>(unicast->decapsulate());
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
        }else if (Ack* msg = dynamic_cast<Ack*>(nm)) {
            handleAck(msg);
            delete msg;
        }
        delete unicast;
    }else if (searchingForPlatoon){
        if(enc->getKind() == BaseProtocol::BEACON_TYPE){
            PlatooningBeacon* pb = check_and_cast<PlatooningBeacon*>(enc);
            int platoonId = ((LeaderPositionHelper*)positionHelper)->isPlatoonLeader(pb->getVehicleId());
            if( platoonId >= 0 ){
                BeliefModel platoonBelief;
                platoonBelief.setBelief("pushplatoon");
                platoonBelief.pushInt(&platoonId);
                double platoonSpeed = pb->getSpeed();
                platoonBelief.pushDouble(&platoonSpeed);
                int leaderId = pb->getVehicleId();
                platoonBelief.pushInt(&leaderId);
                manager->sendInformationToAgents(myId, &platoonBelief);
            }
        }
        delete unicast;
    }
    else {
        GeneralPlexeAgentAppl::handleLowerMsg(msg);
    }
}

void VotingAppl::handleRequestToJoinNegotiation(const RequestJoinPlatoonMessage* msg){
    //Push the knowledge of a potential joiner to the agent
    int joinerId = msg->getVehicleId();
    double joinerSpeed = msg->getPreferedSpeed();
    double joinerPreference = msg->getTolerance();
    BeliefModel jbelief;
    jbelief.setBelief("requestjoin");
    jbelief.pushInt(&joinerId);
    jbelief.pushDouble(&joinerSpeed);
    jbelief.pushDouble(&joinerPreference);
    manager->sendInformationToAgents(myId, &jbelief);
}

void VotingAppl::handleSubmitVote(const SubmitVote* msg){
    if(msg->getPlatoonId() != positionHelper->getPlatoonId()) return;
    if(myId != positionHelper->getLeaderId()) return;
    uint32_t size = msg->getVotesArraySize();
    std::vector<int> votes(size);
    for(uint32_t i = 0; i < size; i++){
        votes[i] = msg->getVotes(i);
    }
    BeliefModel voteSubmission("handle/submit/vote");
    std::string test = msg->getName();
    voteSubmission.pushIntArray(votes);
    manager->sendInformationToAgents(myId, &voteSubmission);
    //Got the vote. Notify of successful delivery
    sendAck("SUBMIT_VOTE", msg->getVehicleId());
    received_votes[msg->getVehicleId()] = true;
}

void VotingAppl::handleNotificationOfResults(const NotifyResults* msg){
    if(msg->getJoinerId() != -1){
        if( (positionHelper->getPlatoonId()) == (msg->getPlatoonId()) ){
            //TODO: Handle insertion of beliefs
        }else if (myId == msg->getJoinerId()){
            BeliefModel result;
            ((VoteManager*)manager)->storeTimeStamp(simTime().dbl() * 1000, VoteManager::TimeStampAction::CHAIR_TO_JOINER_END);
            if(msg->getResult() == 1)
                startJoinManeuver(msg->getPlatoonId(), msg->getVehicleId(), -1);
            else{
                result.setBelief("handlerejection");
                int platoonId = msg->getPlatoonId();
                result.pushInt(&platoonId);
                manager->sendInformationToAgents(myId, &result);
            }
        }
    }else{
        BeliefModel result;
        ((VoteManager*)manager)->storeTimeStamp(simTime().dbl() * 1000, VoteManager::TimeStampAction::TIME_OF_VOTE_END);
        result.setBelief("handle/results");
        int speed = msg->getResult();
        result.pushInt(&speed);
        manager->sendInformationToAgents(myId, &result);
    }
}

void VotingAppl::handleAck(const Ack* msg){
    if(strcmp("SUBMIT_VOTE", msg->getAckType()) == 0){
        cancelAndDelete(awaitAckTimer);
        negotiationState = VoteState::NONE;
    }
}

void VotingAppl::handleSelfMsg(cMessage* msg){
    if (SubmitVote* sv = dynamic_cast<SubmitVote*>(msg)){
        sendUnicast(sv, sv->getDestinationId());
        //delete msg;
    }else if(msg == searchTimer){
        BeliefModel sendRequests;
        sendRequests.setBelief("startrequests");
        searchingForPlatoon = false;
        manager->sendInformationToAgents(myId, &sendRequests);
        delete msg;
    }else if (msg == awaitAckTimer){
        delete msg;
        if(negotiationState == VoteState::AWAITING_ACK_SUBMIT){
            sendVoteSubmition(vote);
            awaitAckTimer = new cMessage("awaitAckTimer");
            //Reschedule to re-send 500ms from now if no ack is received
            scheduleAt(simTime() + 1, awaitAckTimer);
        }
    }else if(msg == voteTimer){
        delete msg;
        voteTimer = NULL;
        //Count how many absentees there are
        int absentees = 0;
        for( const auto& kv_pair : received_votes ){
            if(!kv_pair.second){
                absentees++;
                //Resend the notification of vote to whoever is missing
                sendNotificationOfVoteDirect(election_data, kv_pair.first);
            }
        }
        if(absentees > 0){
            voteTimer = new cMessage("VoteTimer");
            scheduleAt(simTime() + 0.5, voteTimer);
        }
    }else {
        GeneralPlexeAgentAppl::handleSelfMsg(msg);
    }

}

void VotingAppl::handleNotifyVote(const NotifyVote* msg){
    if (positionHelper->isInSamePlatoon(msg->getVehicleId())) { // Verify that it is from this platoon
        BeliefModel voteNotify("handle/vote/notification");
        std::vector<double> contextArgs(msg->getContextArgumentsArraySize() + 1);
        uint32_t size = msg->getCandidatesArraySize();
        std::vector<int> candidates(size);
        for(uint32_t i = 0; i < size; i++){
            candidates[i] = msg->getCandidates(i);
        }
        voteNotify.pushIntArray(candidates);
        switch(msg->getContextId()){
        case CONTEXT_SPEED:
        {
            ((VoteManager*)manager)->storeTimeStamp(simTime().dbl() * 1000, VoteManager::TimeStampAction::CHAIR_TO_MEMBER_END);
            contextArgs[0] = CONTEXT_SPEED;
            break;
        }
        case CONTEXT_JOIN:
        {
            size = contextArgs.capacity();
            contextArgs[0] = CONTEXT_JOIN;
            for(uint32_t i = 1; i < size; i++){
                contextArgs[i] = msg->getContextArguments(i - 1);
            }
            break;
        }
        default:
            throw cRuntimeError("VotingAppl: Invalid context identifier!");
            break;
        }
        voteNotify.pushDoubleArray(contextArgs);
        manager->sendInformationToAgents(myId, &voteNotify);
    }
}

void VotingAppl::sendToAgent(const BeliefModel* bm){
    manager->sendInformationToAgents(myId, bm);
}
