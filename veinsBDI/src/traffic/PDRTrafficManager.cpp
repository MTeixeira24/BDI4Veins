/*
 * PDRTrafficManager.cpp
 *
 *  Created on: Oct 11, 2018
 *      Author: miguel
 */

#include "PDRTrafficManager.h"

Define_Module(PDRTrafficManager);

void PDRTrafficManager::initialize(int stage)
{

    TraCIBaseTrafficManager::initialize(stage);
    vehicleVType = par("vehicleVType").stdstringValue();
    separation = par("separation").intValue();
}

void PDRTrafficManager::scenarioLoaded()
{
    struct Vehicle automated;
    automated.id = findVehicleTypeIndex(vehicleVType);
    automated.speed = 100;
    automated.lane = 0;
    if(separation > -1 ){
        automated.position = separation;
        addVehicleToQueue(0, automated);
    }
    automated.position = 0;
    addVehicleToQueue(0, automated);
}

