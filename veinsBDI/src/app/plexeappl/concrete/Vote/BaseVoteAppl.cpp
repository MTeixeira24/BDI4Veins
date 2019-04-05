/*
 * BaseVoteAppl.cpp
 *
 *  Created on: Apr 5, 2019
 *      Author: miguel
 */

#include "BaseVoteAppl.h"

BaseVoteAppl::BaseVoteAppl() {
    // TODO Auto-generated constructor stub

}

BaseVoteAppl::~BaseVoteAppl() {
    // TODO Auto-generated destructor stub
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
}

