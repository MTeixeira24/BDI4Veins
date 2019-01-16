/*
 * PDRScenario.h
 *
 *  Created on: Oct 11, 2018
 *      Author: miguel
 */

#ifndef SCENARIOS_PDRSCENARIO_H_
#define SCENARIOS_PDRSCENARIO_H_

#include "veins/modules/application/platooning/scenarios/BaseScenario.h"
#include "veins/modules/application/platooning/apps/SimplePlatooningApp.h"
#include "../app/plexeappl/GeneralPlexeAgentAppl.h"

class PDRScenario : public BaseScenario  {
public:
    PDRScenario();
    virtual ~PDRScenario();
    void initialize(int stage) override;
private:
    SimplePlatooningApp* app;
};

#endif /* SCENARIOS_PDRSCENARIO_H_ */
