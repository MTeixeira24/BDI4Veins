/*
 * JoinScenarioAI.h
 *
 *  Created on: 12/09/2018
 *      Author: miguel
 */

#ifndef SCENARIOS_JOINSCENARIOAI_H_
#define SCENARIOS_JOINSCENARIOAI_H_

#include "JoinScenarioNoAI.h"
#include "../app/plexeappl/example/VotingAppl.h"

class JoinScenarioAI : public JoinScenarioNoAI {
public:
    JoinScenarioAI();
    virtual ~JoinScenarioAI();
    virtual void initialize(int stage) override;
protected:
    virtual void handleSelfMsg(cMessage* msg) override;
};

#endif /* SCENARIOS_JOINSCENARIOAI_H_ */
