/*
 * RouteVotingAppl.cpp
 *
 *  Created on: 14/10/2018
 *      Author: miguel
 */

#include "RouteVotingAppl.h"

Define_Module(RouteVotingAppl);

void RouteVotingAppl::initialize(int stage){
    GeneralPlexeAgentAppl::initialize(stage);
    if (stage == 1){
        // connect negotiation application to protocol
        protocol->registerApplication(NEGOTIATION_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"), gate("lowerControlIn"), gate("lowerControlOut"));
        // set initial beliefs
        //set speed from config file
        int desiredSpeed = ((VoteManager*)manager)->getPreferredSpeed(myId);
        //Get the desired path from the config file
        std::vector<int> desiredPath = ((VoteManager*)manager)->getPreferredPath(myId);
        BeliefModel ds("set/prefered/speed");
        ds.pushInt(&desiredSpeed);
        manager->sendInformationToAgents(myId, &ds);
        BeliefModel dp("set/prefered/path");
        dp.pushIntArray(desiredPath);
        manager->sendInformationToAgents(myId, &dp);
        if(getPlatoonRole() == PlatoonRole::NONE) {
            //Set the belief in the agent that they are looking for platoon proposals
            if(par("engageNegotiations").boolValue()){
                BeliefModel bm;
                bm.setBelief("lookforplatoon");
                manager->sendInformationToAgents(myId,&bm);
                searchingForPlatoon = true;
                searchTimer = new cMessage();
                //Search for platoons for a second
                scheduleAt(simTime() + SimTime(1), searchTimer);
            }
        }else{
            //This a vehicle belonging to a platoon
            BeliefModel pbelief;
            pbelief.setBelief("set/initial/beliefs");
            int platoonId = positionHelper->getPlatoonId();
            int leaderId = positionHelper->getLeaderId();
            pbelief.pushInt(&platoonId);
            pbelief.pushInt(&leaderId);
            int platoonSpeed = (positionHelper->getPlatoonSpeed() * 3.6);
            pbelief.pushInt(&platoonSpeed);
            int chair = -1;
            std::vector<int> members = positionHelper->getPlatoonFormation();
            if (getPlatoonRole() == PlatoonRole::LEADER) {
                //This is a leader, push the beliefs that allow an agent to know its role
                chair = platoonId;
                for (uint32_t i = 0; i < members.size(); i++){
                    received_votes[members[i]] = false;
                }
            }
            pbelief.pushInt(&chair);
            pbelief.pushIntArray(members);
            manager->sendInformationToAgents(myId, &pbelief);
        }
    }
}


void RouteVotingAppl::handleLowerMsg(cMessage* msg){
    VotingAppl::handleLowerMsg(msg);
}
