/*
 * BaseAgentAppl.cpp
 *
 *  Created on: Jul 30, 2018
 *      Author: miguel
 */

#include "BaseAgentAppl.h"

Define_Module(BaseAgentAppl);

const simsignalwrap_t BaseAgentAppl::mobilityStateChangedSignal = simsignalwrap_t(MIXIM_SIGNAL_MOBILITY_CHANGE_NAME);

 void BaseAgentAppl::initialize(int stage){
     BaseApplLayer::initialize(stage);
     if(stage == 0){
         myId = getParentModule()->getId();
         //findHost()->subscribe(mobilityStateChangedSignal, this);
         //findHost()->subscribe(parkingStateChangedSignal, this);
         triggerInitialBeliefs = new cMessage("triggerInitialBeliefs");
         scheduleAt(simTime() + 0.001, triggerInitialBeliefs);
     }
     if(stage == 1){
         mobility = Veins::TraCIMobilityAccess().get(getParentModule());
         traci = mobility->getCommandInterface();
         traciVehicle = mobility->getVehicleCommandInterface();


         //Save pointer to LightJason Manager
         manager = Veins::FindModule<LightJasonManager*>::findSubModule(getParentModule()->getParentModule()); //The network is 2 modules up
         if(manager == nullptr){
             throw new cRuntimeError("LightJason Application: No manager found");
         }
         /*
          * Vehicle type must be specified
          */
         std::string aslFile = par("asl_file").stdstringValue();
         manager->subscribeVehicle(this, myId, traciVehicle->getVType(), aslFile);
     }

}

void BaseAgentAppl::sendMessage(uint8_t message_type, const void* args){
    throw new cRuntimeError("Send message not implemented!");
}

void BaseAgentAppl::finish(){
    manager->unsubscribeVehicle(myId);
}

uint8_t BaseAgentAppl::Belief(std::string key){
    return manager->getBeliefId(key);
}


void BaseAgentAppl::handleSelfMsg(cMessage* msg){
    if(msg == triggerInitialBeliefs){
        delete triggerInitialBeliefs;
        setInitialBeliefs();
    }
}
