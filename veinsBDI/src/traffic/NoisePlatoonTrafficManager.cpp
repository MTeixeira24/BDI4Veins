/*
 * NoisePlatoonTrafficManager.cpp
 *
 *  Created on: Oct 12, 2018
 *      Author: miguel
 */

#include "NoisePlatoonTrafficManager.h"

Define_Module(NoisePlatoonTrafficManager);

NoisePlatoonTrafficManager::~NoisePlatoonTrafficManager(){
    cancelAndDelete(addNoise);
}

void NoisePlatoonTrafficManager::initialize(int stage) {

    noiseVType = par("noiseVType").stdstringValue();
    noiseLanes = par("noiseLanes").intValue();
    addNoise = new cMessage("addNoise");
    JoinBDITrafficManager::initialize(stage);
   // scheduleAt(simTime() + 0.5, addNoise);
}

void NoisePlatoonTrafficManager::scenarioLoaded()  {
    /*Compute the estimated size of the platoon, in order to reserve space*/
    int platoonSize = int(par("platoonSize").doubleValue());
    double platoonSpeed = par("platoonInsertSpeed").doubleValue() / 3.6;
    //int plength = (platoonSize * 4 + (platoonSize - 1) * (platoonInsertDistance + platoonInsertHeadway * platoonSpeed) + 4) + (platoonLeaderHeadway * platoonInsertSpeed->doubleValue() / 3.6);
    int platoonLength = (platoonSize * 4 + (platoonSize - 1) * (platoonInsertDistance + platoonInsertHeadway * platoonSpeed) + 4) + (platoonLeaderHeadway * platoonInsertSpeed->doubleValue() / 3.6);
    int maxDistance = 500 + (platoonLength/2);
    int minDistance = 400 - (platoonLength/2);


    int length = 1000, insertPosition = 1000;
    int vehicleDensity = par("background_density").intValue();
    int vehiclePerLane = vehicleDensity/noiseLanes;

    int base_speed = baseSpeed;
    struct Vehicle automated;
    vehTypeId = findVehicleTypeIndex(noiseVType);
    automated.id = vehTypeId;

    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> distance_distribution(30,8);
    std::normal_distribution<double> speed_distribution(10,2);

    for(int i = 0; i < noiseLanes; i++){
        insertPosition = length;
        //int insertionCount = i < nLanes ? vehiclesInPlatoonLane : vehiclePerLane;
        for(int v = 0; v < vehiclePerLane; v++){
            int speed = base_speed + speed_distribution(gen);
            //If the platoon will be generated in this lane, break before vehicles are generated on top of each other
            //otherwise prevent negative values
            if(i < nLanes || (joinerLane > -1 && i == joinerLane )){
                if(insertPosition < maxDistance && insertPosition > minDistance) {
                    insertPosition = insertPosition - (length/vehiclePerLane);
                    continue;
                }
            }else{
                if(insertPosition < 4) break;
            }
            automated.speed = speed / 3.6;
            automated.position = insertPosition;
            automated.lane = i;
            addVehicleToQueue(0, automated);
            insertPosition = insertPosition - (length/vehiclePerLane);
        }
        base_speed += 10;
    }

    JoinBDITrafficManager::injectPlatoon();
}

void NoisePlatoonTrafficManager::handleSelfMsg(cMessage* msg)  {
    TraCIBaseTrafficManager::handleSelfMsg(msg);
        if(msg == addJoiner){
            injectJoiner();
            suppressInjections = false;
        }else if(msg == addNoise){
            if(suppressInjections){
                if(injectLane < nLanes || (joinerLane > -1 && injectLane == joinerLane )){
                    int originalLane = injectLane;
                    //Find a lane not occupied by joiners or platoons
                    for(int i = 0; i < noiseLanes; i++){
                        if(i < nLanes || (joinerLane > -1 && i == joinerLane )){
                            continue;
                        }else{
                            injectLane = i;
                            break;
                        }
                    }
                    if(originalLane != injectLane){
                        injectVehicle(injectLane, baseSpeed + (10*injectLane));
                        injectLane = (injectLane + 1) % noiseLanes;
                    }
                }
            }else{
                injectVehicle(injectLane, baseSpeed + (10*injectLane));
                injectLane = (injectLane + 1) % noiseLanes;
            }
            scheduleAt(simTime() + 1, addNoise);
        }
}

void NoisePlatoonTrafficManager::injectVehicle(int lane, int speed){
    struct Vehicle automated;
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> speed_distribution(10,2);
    automated.id = vehTypeId;
    automated.speed = (speed + speed_distribution(gen)) / 3.6;
    automated.position = 4;
    automated.lane = lane;
    addVehicleToQueue(0, automated);
}
