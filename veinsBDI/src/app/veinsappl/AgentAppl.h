/*
 * AgentAppl.h
 *
 *  Created on: Jul 30, 2018
 *      Author: miguel
 */

#ifndef AGENTAPPL_H_
#define AGENTAPPL_H_

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include "../../base/manager/LightJasonManager.h"
using Veins::TraCIMobility;
using Veins::TraCICommandInterface;
//using Veins::AnnotationManager; //Add for annotations
class LightJasonManager;
class AgentAppl : public Veins::BaseWaveApplLayer {
public:
    virtual void initialize(int stage);
    virtual void finish();
    //~AgentAppl();
    virtual void receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj, cObject* details);
protected:
    TraCIMobility* mobility;
    TraCICommandInterface* traci;
    TraCICommandInterface::Vehicle* traciVehicle;
    simtime_t lastSent; //last time this sent a message
    LightJasonManager* manager;

    virtual void onWSM(Veins::WaveShortMessage* wsm);
    virtual void onBeacon(Veins::WaveShortMessage* wsm);
    virtual void handlePositionUpdate(cObject* obj);
    void sendMessage(std::string msg);
    virtual void sendWSM(Veins::WaveShortMessage* wsm);
public:
    void changeSpeed(double speed);
};

#endif /* AGENTAPPL_H_ */
