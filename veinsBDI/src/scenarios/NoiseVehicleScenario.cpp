/*
 * NoiseVehicleScenario.cpp
 *
 *  Created on: 13/10/2018
 *      Author: miguel
 */

#include "NoiseVehicleScenario.h"

Define_Module(NoiseVehicleScenario);

void NoiseVehicleScenario::initialize(int stage)
{
    BaseScenario::initialize(stage);
    if(stage == 1){
        traciVehicle->setFixedLane(traciVehicle->getLaneIndex(), true);
        traciVehicle->setActiveController(Plexe::ACC);
        traciVehicle->setCruiseControlDesiredSpeed(mobility->getSpeed());
        positionHelper->setPlatoonId(-1);
        positionHelper->setIsLeader(false);
        positionHelper->setPlatoonLane(-1);
    }
}
