/*
 * JoinScenarioAI.cpp
 *
 *  Created on: 12/09/2018
 *      Author: miguel
 */

#include "JoinScenarioAI.h"

Define_Module(JoinScenarioAI);

JoinScenarioAI::JoinScenarioAI() {

}

JoinScenarioAI::~JoinScenarioAI() {
}

void JoinScenarioAI::initialize(int stage)
{

    JoinScenarioNoAI::initialize(stage);
    if(stage == 0){

    }

    if(stage == 1){

    }
}

void JoinScenarioAI::handleSelfMsg(cMessage* msg)
{
    // this takes care of feeding data into CACC and reschedule the self message
    BaseScenario::handleSelfMsg(msg);
}
