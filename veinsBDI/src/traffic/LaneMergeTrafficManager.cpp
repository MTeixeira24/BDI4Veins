/*
 * LaneMergeTrafficManager.cpp
 *
 *  Created on: 21/08/2018
 *      Author: miguel
 */

#include "LaneMergeTrafficManager.h"

Define_Module(LaneMergeTrafficManager);

void LaneMergeTrafficManager::initialize(int stage){
    PlatoonsTrafficManager::initialize(stage);

    if (stage == 0) {
        insertArterialMessage = new cMessage("");
        scheduleAt(platoonInsertTime + SimTime(1), insertArterialMessage); //Insert a vehicle in the arterial lane one second after platoon insertion
    }
}

void LaneMergeTrafficManager::handleSelfMsg(cMessage* msg)
{

    PlatoonsTrafficManager::handleSelfMsg(msg);

    if (msg == insertArterialMessage) {
        insertArterial();
    }
}

void LaneMergeTrafficManager::insertArterial()
{
    automated.position = 5;
    automated.lane = 0;
    addVehicleToQueue(2, automated);
}
LaneMergeTrafficManager::~LaneMergeTrafficManager()
{
    cancelAndDelete(insertArterialMessage);
    insertArterialMessage = nullptr;
}
