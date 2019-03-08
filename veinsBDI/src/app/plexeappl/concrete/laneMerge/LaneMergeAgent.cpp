/*
 * LaneMergeAgent.cpp
 *
 *  Created on: Feb 27, 2019
 *      Author: miguel
 */

#include "LaneMergeAgent.h"

Define_Module(LaneMergeAgent);

LaneMergeAgent::LaneMergeAgent() {
}

LaneMergeAgent::~LaneMergeAgent() {
}

void LaneMergeAgent::initialize(int stage){
    GeneralPlexeAgentAppl::initialize(stage);
}

void LaneMergeAgent::handleLowerMsg(cMessage* msg){

    UnicastMessage* unicast = check_and_cast<UnicastMessage*>(msg);

    cPacket* enc = unicast->getEncapsulatedPacket();
    ASSERT2(enc, "received a UnicastMessage with nothing inside");

    GeneralPlexeAgentAppl::handleLowerMsg(msg);
}

void LaneMergeAgent::handleSelfMsg(cMessage* msg){
    GeneralPlexeAgentAppl::handleSelfMsg(msg);
}
