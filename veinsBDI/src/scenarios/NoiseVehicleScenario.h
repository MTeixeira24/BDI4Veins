/*
 * NoiseVehicleScenario.h
 *
 *  Created on: 13/10/2018
 *      Author: miguel
 */

#ifndef SCENARIOS_NOISEVEHICLESCENARIO_H_
#define SCENARIOS_NOISEVEHICLESCENARIO_H_

#include "veins/modules/application/platooning/scenarios/BaseScenario.h"

class NoiseVehicleScenario : public BaseScenario {
public:
    NoiseVehicleScenario(){};
    virtual ~NoiseVehicleScenario(){}
    virtual void initialize(int stage) override;;
};

#endif /* SCENARIOS_NOISEVEHICLESCENARIO_H_ */
