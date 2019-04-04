/*
 * LaneTrafficManager.cpp
 *
 *  Created on: 08/03/2019
 *      Author: miguel
 */

#include "LaneTrafficManager.h"

Define_Module(LaneTrafficManager);

LaneTrafficManager::~LaneTrafficManager() {
    if(!injectTimer){
        cancelAndDelete(injectTimer);
    }
}

void LaneTrafficManager::initialize(int stage){
    TraCIBaseTrafficManager::initialize(stage);

    continuousInjection = par("continuousInjection").boolValue();
    vPairs = par("vPairs").intValue();
    lanePairs = par("lanePairs").intValue();
    injectionTimeHeadway = par("injectionTimeHeadway").intValue();
    vType = par("vType").stdstringValue();
    travelSpeed = par("travelSpeed").intValue();

    if(continuousInjection){
        injectTimer = new cMessage("injectTimer");
    }
}

void LaneTrafficManager::scenarioLoaded(){
    injectInitialPairs();
}

void LaneTrafficManager::handleSelfMsg(cMessage* msg){

    TraCIBaseTrafficManager::handleSelfMsg(msg);
    if(msg == injectTimer){
        injectPair();
        scheduleAt(simTime() + injectionTimeHeadway, injectTimer);
    }
}

void LaneTrafficManager::injectInitialPairs(){
    int vehTypeId = findVehicleTypeIndex(vType);
    struct Vehicle automated;


    int totalLength = (vPairs * 22) + 10;
    for(int i = 0; i < vPairs; i++){
        //for each lane pair, generate a pair
        for(int j = 0; j < lanePairs; j++){
            for(int z = 0; z < 2; z++){
                automated.id = vehTypeId;
                automated.speed = travelSpeed;
                automated.position = totalLength - 6*z;
                automated.lane = 2*j + z;
                addVehicleToQueue(0, automated);
            }
        }
        totalLength -= 22;
    }
    if(continuousInjection)
        scheduleAt(simTime() + injectionTimeHeadway, injectTimer);
}

void LaneTrafficManager::injectPair(){
    int vehTypeId = findVehicleTypeIndex(vType);
    struct Vehicle automated;
    //for each lane pair, generate a pair
    for(int j = 0; j < lanePairs; j++){
        for(int z = 0; j < 2; j++){
            automated.id = vehTypeId;
            automated.speed = travelSpeed;
            automated.position = 10 - 5*z;
            automated.lane = 2*j + z;
            addVehicleToQueue(0, automated);
        }
    }
}


