/*
 * BaseAgentAppl.h
 *
 *  Created on: Jul 30, 2018
 *      Author: miguel
 */

#ifndef BASEAGENTAPPL_H_
#define BASEAGENTAPPL_H_

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include "../../base/manager/LightJasonManager.h"
using Veins::TraCIMobility;
using Veins::TraCICommandInterface;

class LightJasonManager;
class BaseAgentAppl : public Veins::BaseWaveApplLayer {
public:
    virtual void initialize(int stage);
    virtual void finish();
    virtual void receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj, cObject* details){};
    virtual void sendMessage(uint8_t message_type, const void* args);
protected:
    TraCIMobility* mobility;
    TraCICommandInterface* traci;
    TraCICommandInterface::Vehicle* traciVehicle;
    simtime_t lastSent; //last time this sent a message
    LightJasonManager* manager;
    virtual void onWSM(Veins::WaveShortMessage* wsm){

    }
    virtual void handlePositionUpdate(cObject* obj){
        BaseWaveApplLayer::handlePositionUpdate(obj);
    }
    virtual void handleLowerMsg(cMessage* msg){}
    virtual void handleSelfMsg(cMessage* msg){}
    virtual void handleLowerControl(cMessage* msg){}

    //virtual void startManeuver();

public:
    virtual void changeSpeed(double speed);
};

#endif /* BASEAGENTAPPL_H_ */
