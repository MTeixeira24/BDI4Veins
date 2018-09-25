/*
 * JoinScenarioNoAI.h
 *
 *  Created on: Sep 7, 2018
 *      Author: miguel
 */

#ifndef SCENARIOS_JOINSCENARIONOAI_H_
#define SCENARIOS_JOINSCENARIONOAI_H_

#include "veins/modules/application/platooning/scenarios/BaseScenario.h"
#include "../app/plexeappl/GeneralPlexeAgentAppl.h"

class JoinScenarioNoAI : public BaseScenario {
public:
    JoinScenarioNoAI();
    virtual ~JoinScenarioNoAI();
    virtual void initialize(int stage) override;
protected:
    void handleSelfMsg(cMessage* msg);
    std::string platooningVType;
    GeneralPlexeAgentAppl* app;
    cMessage* startManeuver;
    cMessage* test;
};

#endif /* SCENARIOS_JOINSCENARIONOAI_H_ */
