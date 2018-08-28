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
        mergerVtype = par("mergerVType").stdstringValue();
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
    merger.id = findVehicleTypeIndex(mergerVtype);
    merger.lane = 0;
    merger.position = 8;
    merger.speed = platoonInsertSpeed / 3.6;
    addVehicleToQueue(2, merger);
}
LaneMergeTrafficManager::~LaneMergeTrafficManager()
{
    cancelAndDelete(insertArterialMessage);
    insertArterialMessage = nullptr;
}
