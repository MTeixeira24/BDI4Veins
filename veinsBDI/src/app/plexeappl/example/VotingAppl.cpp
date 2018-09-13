/*
 * VotingAppl.cpp
 *
 *  Created on: 12/09/2018
 *      Author: miguel
 */

#include "VotingAppl.h"

Define_Module(VotingAppl);

VotingAppl::VotingAppl() {
}

VotingAppl::~VotingAppl() {
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
            bm.setBelief("foundplatoon");
            int platoonId = 0;
            int leaderId = 0;
            bm.pushInt(&platoonId);
            bm.pushInt(&leaderId);
            manager->sendInformationToAgents(myId,&bm);
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
    }
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

void VotingAppl::sendToAgent(const BeliefModel* bm){
    manager->sendInformationToAgents(myId, bm);
}
