/*
 * PDRScenario.h
 *
 *  Created on: Oct 11, 2018
 *      Author: miguel
 */

#ifndef SCENARIOS_PDRSCENARIO_H_
#define SCENARIOS_PDRSCENARIO_H_

#include "veins/modules/application/platooning/scenarios/BaseScenario.h"

class PDRScenario : public BaseScenario  {
public:
    PDRScenario();
    virtual ~PDRScenario();
    void initialize(int stage) override;
};

#endif /* SCENARIOS_PDRSCENARIO_H_ */
