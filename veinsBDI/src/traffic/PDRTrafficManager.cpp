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

    noiseVType = par("noiseVType").stdstringValue();
    flow = par("flow").intValue();
}

void PDRTrafficManager::scenarioLoaded()
{
    int startingPoint = 500;
    struct Vehicle automated;
    automated.id = findVehicleTypeIndex(vehicleVType);
    automated.speed = 100;
    automated.lane = 0;
    if(separation > -1 ){
        automated.position = startingPoint + (separation/2);
        addVehicleToQueue(0, automated);
    }
    automated.position = startingPoint - (separation/2);
    addVehicleToQueue(0, automated);
    if(flow > 0){
        injectTrafficTimer = new cMessage("injectTraffic");
        scheduleAt(simTime() + 0.1, injectTrafficTimer);
    }
}

void PDRTrafficManager::injectTraffic(){
    int initialVehicleCount = flow/2;
    int length = 1000;
    int base_speed = 100;
    struct Vehicle automated;
    int vehTypeId = findVehicleTypeIndex(noiseVType);
    automated.id = vehTypeId;

    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> distance_distribution(30,8);
    std::normal_distribution<double> speed_distribution(10,2);

    for(int i = 1; i < 3; i++){
        int insertPosition = length - speed_distribution(gen);
        for(int v = 0; v < initialVehicleCount; v++){
            int speed = base_speed;
            int separation = distance_distribution(gen);
            insertPosition -= separation;
            if(insertPosition < 4) break;
            automated.speed = speed;
            automated.position = insertPosition;
            automated.lane = i;
            addVehicleToQueue(0, automated);
            insertPosition -= 8;
        }
    }
}

void PDRTrafficManager::handleSelfMsg(cMessage* msg){
    if(msg == injectTrafficTimer){
        injectTraffic();
        delete injectTrafficTimer;
    }
    else
        TraCIBaseTrafficManager::handleSelfMsg(msg);
}
