/*
 * PDRScenario.cpp
 *
 *  Created on: Oct 11, 2018
 *      Author: miguel
 */

#include "PDRScenario.h"
#include "veins/modules/application/platooning/utilities/DynamicPositionManager.h"

Define_Module(PDRScenario);

PDRScenario::PDRScenario() {
}

PDRScenario::~PDRScenario() {
}

void PDRScenario::initialize(int stage){
    BaseScenario::initialize(stage);
}
