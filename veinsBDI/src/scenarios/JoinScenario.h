/*
 * JoinScenario.h
 *
 *  Created on: Sep 7, 2018
 *      Author: miguel
 */

#ifndef SCENARIOS_JOINSCENARIO_H_
#define SCENARIOS_JOINSCENARIO_H_

#include "veins/modules/application/platooning/scenarios/BaseScenario.h"
#include "../app/plexeappl/GeneralPlexeAgentAppl.h"

class JoinScenario : public BaseScenario {
public:
    JoinScenario(){};
    virtual ~JoinScenario(){};
    virtual void initialize(int stage) override;
protected:
    void handleSelfMsg(cMessage* msg);
    std::string platooningVType;
    GeneralPlexeAgentAppl* app;
    cMessage* startManeuver;
    cMessage* test;
};

#endif /* SCENARIOS_JOINSCENARIO_H_ */
