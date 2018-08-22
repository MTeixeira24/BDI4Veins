/*
 * BaseAgentAppl.cpp
 *
 *  Created on: Jul 30, 2018
 *      Author: miguel
 */

#include "BaseAgentAppl.h"



 void BaseAgentAppl::initialize(int stage){
     BaseApplLayer::initialize(stage); //TODO: Header subclasses BaseWave. Maybe change that?
}

void BaseAgentAppl::changeSpeed(double speed){
    EV << "Slowing down to speed on agent command\n";
    traciVehicle->setMaxSpeed(speed);
}

void BaseAgentAppl::finish(){
    manager->unsubscribeVehicle(myId);
}
