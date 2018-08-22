/*
 * BaseVeinsAgentAppl.cpp
 *
 *  Created on: 22/08/2018
 *      Author: miguel
 */

#include "BaseVeinsAgentAppl.h"

using Veins::TraCIMobilityAccess;
using Veins::AnnotationManagerAccess;

Define_Module(BaseVeinsAgentAppl);

 void BaseVeinsAgentAppl::initialize(int stage){
     BaseAgentAppl::initialize(stage);
    if(stage == 0){
        //setup veins pointers
        mobility = TraCIMobilityAccess().get(getParentModule());
        traci = mobility->getCommandInterface();
        traciVehicle = mobility->getVehicleCommandInterface();
        lastSent = simTime();
        manager = Veins::FindModule<LightJasonManager*>::findSubModule(getParentModule()->getParentModule()); //The network is 2 modules up
        if(manager == nullptr){
            throw new cRuntimeError("LightJason Application: No manager found");
        }
        manager->subscribeVehicle(this, myId);
        //Save pointer to LightJason Manager
        //Call registration service with node identifier
        //traciVehicle->setLaneChangeMode(0);
    }
}

 void BaseVeinsAgentAppl::receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj, cObject* details){
    Enter_Method_Silent();
    if(signalID == mobilityStateChangedSignal){
        handlePositionUpdate(obj);
    }
}

 void BaseVeinsAgentAppl::onWSM(Veins::WaveShortMessage* wsm){
    //Receive a message with a target speed, slow down to that speed
    std::string msg = wsm->getWsmData();
    std::string del ("|");
    std::string speed = msg.substr(0, msg.find(del));
    std::string sid= msg.substr(msg.find(del) + 1, msg.length());
    float message_speed = atof( speed.c_str() );
    //int id = std::atoi( sid.c_str() );
    //float message_speed = atof(wsm->getWsmData());
    EV << "Slowing down to speed " << message_speed << "\n";
    //traciVehicle->slowDown(message_speed, 100); //slow down over 1s
}

 void BaseVeinsAgentAppl::onBeacon(Veins::WaveShortMessage* wsm){
     //Receive a message with a target speed, slow down to that speed
     float message_speed = atof(wsm->getWsmData());
     EV << "Slowing down to speed " << message_speed << "\n";
     traciVehicle->slowDown(message_speed, 1000); //slow down over 1ss
}

 void BaseVeinsAgentAppl::handlePositionUpdate(cObject* obj){
     BaseAgentAppl::handlePositionUpdate(obj);
    manager->sendInformationToAgents(myId, "speed", std::to_string(mobility->getSpeed()));
    //sends message every 5 seconds
    if(simTime() - lastSent >= 5){
        std::string message = std::to_string(mobility->getSpeed());
        sendMessage(message);
        lastSent = simTime();
    }
}

void BaseVeinsAgentAppl::sendMessage(std::string msg){
    t_channel channel = dataOnSch ? type_SCH : type_CCH;
    Veins::WaveShortMessage *wsm = new Veins::WaveShortMessage("data", channel);
    std::string m_msg = std::to_string(myId);
    wsm->setWsmData((msg + "|" + m_msg).c_str());
    sendWSM(wsm);
}

 void BaseVeinsAgentAppl::sendWSM(Veins::WaveShortMessage* wsm){
    sendDown(wsm); //message delay
}

void BaseVeinsAgentAppl::changeSpeed(double speed){
    EV << "Slowing down to speed on agent command\n";
    traciVehicle->setMaxSpeed(speed);
}

void BaseVeinsAgentAppl::finish(){
    manager->unsubscribeVehicle(myId);
}
