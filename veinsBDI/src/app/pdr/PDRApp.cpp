/*
 * PDRApp.cpp
 *
 *  Created on: Jan 13, 2019
 *      Author: miguel
 */

#include "PDRApp.h"
#include "veins/modules/application/platooning/UnicastProtocol.h"
#include "veins/modules/application/platooning/protocols/BaseProtocol.h"
#include "veins/modules/mobility/traci/TraCIColor.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
Define_Module(PDRApp);

PDRApp::PDRApp() {
    sentPackets = 0;
    receivedPackets = 0;
}

PDRApp::~PDRApp() {
    cancelAndDelete(sendTimer);
}

void PDRApp::finish(){
    BaseApp::finish();
    if(myId == 0)
        recordScalar("#sent", sentPackets);
    if(myId == 1)
        recordScalar("#received", receivedPackets);
}

void PDRApp::initialize(int stage){
    BaseApp::initialize(stage);

    if (stage == 1) {
        // connect maneuver application to protocol
        protocol->registerApplication(pdrtype, gate("lowerLayerIn"), gate("lowerLayerOut"), gate("lowerControlIn"), gate("lowerControlOut"));
        if(myId == 0){
            sendTimer = new cMessage("sendTimer");
            scheduleAt(simTime() + 0.1, sendTimer);
        }
    }
}

void PDRApp::sendUnicast(cPacket* msg, int destination){
    Enter_Method_Silent();
    take(msg);
    UnicastMessage* unicast = new UnicastMessage("UnicastMessage", msg->getKind());
    unicast->setDestination(destination);
    unicast->setChannel(Channels::CCH);
    unicast->encapsulate(msg);
    sendDown(unicast);
}

void PDRApp::handleLowerMsg(cMessage* msg){
    UnicastMessage* unicast = check_and_cast<UnicastMessage*>(msg);

    cPacket* enc = unicast->getEncapsulatedPacket();
    ASSERT2(enc, "received a UnicastMessage with nothing inside");

    if (enc->getKind() == pdrtype) {
        //PDRMessage* pdr = check_and_cast<PDRMessage*>(unicast->decapsulate());
        receivedPackets++;
        //ManeuverMessage* mm = check_and_cast<ManeuverMessage*>(unicast->decapsulate());
        /*if (UpdatePlatoonFormation* msg = dynamic_cast<UpdatePlatoonFormation*>(mm)) {
            handleUpdatePlatoonFormation(msg);
            delete msg;
        }*/
        delete unicast;
    }
    else {
        BaseApp::handleLowerMsg(msg);
    }

}

void PDRApp::sendPDR(){
    PDRMessage* msg = new PDRMessage("PDR");
    msg->setKind(pdrtype);
    msg->setVehicleId(myId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
    msg->setDestinationId(1);
    sendUnicast(msg, 1);
}

void PDRApp::handleSelfMsg(cMessage* msg){
    if (msg == sendTimer) {
        sendPDR();
        sentPackets++;
        scheduleAt(simTime() + 0.1, sendTimer);
    }
    else {
        BaseApp::handleSelfMsg(msg);
    }
}
