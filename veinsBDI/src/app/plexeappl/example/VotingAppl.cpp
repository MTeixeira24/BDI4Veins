/*
 * VotingAppl.cpp
 *
 *  Created on: 12/09/2018
 *      Author: miguel
 */

#include "VotingAppl.h"

Define_Module(VotingAppl);


VotingAppl::~VotingAppl() {
    if(searchTimer != nullptr)
        delete searchTimer;
}

void VotingAppl::initialize(int stage){
    GeneralPlexeAgentAppl::initialize(stage);
    if (stage == 1){
        // connect negotiation application to protocol
        protocol->registerApplication(NEGOTIATION_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"), gate("lowerControlIn"), gate("lowerControlOut"));
        // set initial beliefs;
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
                BeliefModel platoonSpeedBelief;
                platoonSpeedBelief.setBelief("platoonspeed");
                double platoonSpeed = (positionHelper->getPlatoonSpeed() * 3.6);
                platoonSpeedBelief.pushDouble(&platoonSpeed);
                manager->sendInformationToAgents(myId, &platoonSpeedBelief);
                std::vector<int> members = positionHelper->getPlatoonFormation();
                for (uint32_t i = 0; i < members.size(); i++){
                    BeliefModel mbelief;
                    mbelief.setBelief("addmember");
                    int member = members[i];
                    mbelief.pushInt(&member);
                    manager->sendInformationToAgents(myId, &mbelief);
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
            }
        }
        /*std::string test1 = getParentModule()->getFullName();
        int test3 = getParentModule()->getId();
        int test2 = myId;
        int buf = myId;*/
    }
}

void VotingAppl::sendNotificationOfSpeedVote(std::vector<int>& candidates){
    NotifyVote* msg = new NotifyVote("NotifyVote");
    msg->setCandidatesArraySize(candidates.size());
    msg->setContext("handle/speed/vote/notification");
    msg->setKind(NEGOTIATION_TYPE);
    for(uint32_t i = 0; i < candidates.size(); i++){
        msg->setCandidates(i, candidates[i]);
    }
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

void VotingAppl::sendVoteSubmition(int vote){
    Enter_Method_Silent();
    SubmitVote* msg = new SubmitVote("SubmitVote");
    int leaderId = positionHelper->getLeaderId();
    msg->setKind(NEGOTIATION_TYPE);
    msg->setPlatoonId(positionHelper->getPlatoonId());
    msg->setVehicleId(myId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
    msg->setDestinationId(leaderId);
    msg->setVote(vote);
    int position = positionHelper->getPosition();
    scheduleAt(simTime() + 0.1*position, msg);
    //if(myId == 1 || myId == 2 || myId == 3)
    //    sendUnicast(msg, leaderId);
}

void VotingAppl::sendNotificationOfJoinVote(double preferedspeed, double tolerance){
    NotificationOfJoinVote* msg = new NotificationOfJoinVote("NotificationOfJoinVote");
    msg->setKind(NEGOTIATION_TYPE);
    msg->setVehicleId(myId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
    msg->setDestinationId(-1);
    msg->setPreferedSpeed(preferedspeed);
    msg->setTolerance(tolerance);
    double platoonSpeed = (positionHelper->getPlatoonSpeed() * 3.6);
    double joinerSpeed = preferedspeed;
    double joinerPreference = tolerance;
    BeliefModel jbelief;
    jbelief.setBelief("openvotetojoin");
    jbelief.pushDouble(&joinerSpeed);
    jbelief.pushDouble(&joinerPreference);
    jbelief.pushDouble(&platoonSpeed);
    manager->sendInformationToAgents(myId, &jbelief);
    sendUnicast(msg, -1);
}

void VotingAppl::sendVoteResults(int joinerId, int results){
    NotifyResults* msg = new NotifyResults("NotifyResults");
    int platoonId = positionHelper->getPlatoonId();
    msg->setKind(NEGOTIATION_TYPE);
    msg->setVehicleId(myId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
    msg->setDestinationId(-1);
    msg->setResult(results);
    msg->setJoinerId(joinerId);
    msg->setPlatoonId(platoonId);
    sendUnicast(msg, -1);

}

void VotingAppl::onPlatoonBeacon(const PlatooningBeacon* pb){
    //TODO: If this is a beacon for voting handle it
    GeneralPlexeAgentAppl::onPlatoonBeacon(pb);
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
        }else if (NotificationOfJoinVote* msg = dynamic_cast<NotificationOfJoinVote*>(nm)) {
            handleNotificationOfJoinVote(msg);
            delete msg;
        }else if (SubmitVote* msg = dynamic_cast<SubmitVote*>(nm)) {
            handleSubmitVote(msg);
            delete msg;
        }else if (NotifyResults* msg = dynamic_cast<NotifyResults*>(nm)) {
            handleNotificationOfResults(msg);
            delete msg;
        }else if (NotifyVote* msg = dynamic_cast<NotifyVote*>(nm)) {
            handleNotifyVote(msg);
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
    int msgPid = msg->getPlatoonId();
    int myPid = positionHelper->getPlatoonId();
    bool r1 = msgPid == myPid;
    int mid = myId;
    int lid = positionHelper->getLeaderId();
    bool r2 = mid == lid;

    if( r1 && r2 ){
        int vote = msg->getVote();
        int voter = msg->getVehicleId();
        BeliefModel submitVote;
        submitVote.setBelief("submitvote");
        submitVote.pushInt(&voter);
        submitVote.pushInt(&vote);
        manager->sendInformationToAgents(myId, &submitVote);
    }
}

void VotingAppl::handleNotificationOfJoinVote(const NotificationOfJoinVote* msg){
    if (positionHelper->isInSamePlatoon(msg->getVehicleId())) { // Verify that it is from this platoon
        double joinerSpeed = msg->getPreferedSpeed();
        double joinerPreference = msg->getTolerance();
        double platoonSpeed = (positionHelper->getPlatoonSpeed() * 3.6);
        BeliefModel jbelief;
        jbelief.setBelief("openvotetojoin");
        jbelief.pushDouble(&joinerSpeed);
        jbelief.pushDouble(&joinerPreference);
        jbelief.pushDouble(&platoonSpeed);
        manager->sendInformationToAgents(myId, &jbelief);
    }
}

void VotingAppl::handleNotificationOfResults(const NotifyResults* msg){
    if( (positionHelper->getPlatoonId()) == (msg->getPlatoonId()) ){
        //TODO: Handle insertion of beliefs
    }else if (myId == msg->getJoinerId()){
        BeliefModel result;
        if(msg->getResult() == 1)
            startJoinManeuver(msg->getPlatoonId(), msg->getVehicleId(), -1);
        else{
            result.setBelief("handlerejection");
            int platoonId = msg->getPlatoonId();
            result.pushInt(&platoonId);
            manager->sendInformationToAgents(myId, &result);
        }
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
    }else {
        GeneralPlexeAgentAppl::handleSelfMsg(msg);
    }

}

void VotingAppl::handleNotifyVote(const NotifyVote* msg){
    uint32_t size = msg->getCandidatesArraySize();
    std::vector<int> candidates(size);
    for(uint32_t i = 0; i < size; i++){
        candidates[i] = msg->getCandidates(i);
    }
    BeliefModel voteNotify(msg->getContext());
    voteNotify.pushIntArray(candidates);
    manager->sendInformationToAgents(myId, &voteNotify);
}

void VotingAppl::sendToAgent(const BeliefModel* bm){
    manager->sendInformationToAgents(myId, bm);
}
