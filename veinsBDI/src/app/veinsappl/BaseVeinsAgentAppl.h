/*
 * BaseVeinsAgentAppl.h
 *
 *  Created on: 22/08/2018
 *      Author: miguel
 */

#ifndef APP_VEINSAPPL_BASEVEINSAGENTAPPL_H_
#define APP_VEINSAPPL_BASEVEINSAGENTAPPL_H_

#include "../baseappl/BaseAgentAppl.h"

class BaseVeinsAgentAppl : public BaseAgentAppl {
public:
    BaseVeinsAgentAppl(){}
    virtual ~BaseVeinsAgentAppl(){}
    virtual void initialize(int stage);
    virtual void finish();
    //~AgentAppl();
    virtual void receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj, cObject* details);
protected:

    virtual void onWSM(Veins::WaveShortMessage* wsm);
    virtual void onBeacon(Veins::WaveShortMessage* wsm);
    virtual void handlePositionUpdate(cObject* obj);
    void sendMessage(std::string msg);
    virtual void sendWSM(Veins::WaveShortMessage* wsm);
public:
    void changeSpeed(double speed);
};

#endif /* APP_VEINSAPPL_BASEVEINSAGENTAPPL_H_ */
