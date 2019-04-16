/*
 * JoinBDITrafficManager.cpp
 *
 *  Created on: Sep 7, 2018
 *  Based on: RingTrafficManager.cc
 *      Author: miguel
 */

#include "JoinBDITrafficManager.h"

Define_Module(JoinBDITrafficManager);

JoinBDITrafficManager::~JoinBDITrafficManager(){
    cancelAndDelete(addJoiner);
}

void JoinBDITrafficManager::initialize(int stage)
{

    TraCIBaseTrafficManager::initialize(stage);


    DynamicPositionManager& positions = DynamicPositionManager::getInstance();
    positions.platoons.clear();
    positions.positions.clear();
    positions.vehToPlatoons.clear();

    platoonSize = &par("platoonSize");
    numJoiners = par("numJoiners").intValue();
    nPlatoons = par("nPlatoons").intValue();
    nLanes = par("nLanes").intValue();
    platoonInsertSpeed = &par("platoonInsertSpeed");
    platoonInsertDistance = par("platoonInsertDistance").doubleValue();
    platoonInsertHeadway = par("platoonInsertHeadway").doubleValue();
    platoonLeaderHeadway = par("platoonLeaderHeadway").doubleValue();
    platooningVType = par("platooningVType").stdstringValue();
    joinerLane = par("joinerLane").intValue();
    addJoiner = new cMessage();
    scheduleAt(simTime() + 2.75, addJoiner);




    noiseVType = par("noiseVType").stdstringValue();
    noiseLanes = par("noiseLanes").intValue();
}

void JoinBDITrafficManager::scenarioLoaded()
{
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

    int base_speed = 110;
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
    injectPlatoon();
}

void JoinBDITrafficManager::handleSelfMsg(cMessage* msg){
    TraCIBaseTrafficManager::handleSelfMsg(msg);
    if(msg == addJoiner){
        injectJoiner();
    }
}

void JoinBDITrafficManager::injectPlatoon(){
    int vehTypeId = findVehicleTypeIndex(platooningVType);
    struct Vehicle automated;

    // map from lane index to a vector with all platoon sizes
    std::map<int, std::vector<Platoon>> platoons;
    // total vehicle length for each lane
    std::vector<double> lengths;

    for (int l = 0; l < nLanes; l++) {
        platoons[l] = std::vector<Platoon>();
        lengths.push_back(0);
    }

    int l = 0;
    for (int p = 0; p < nPlatoons; p++) {
        Platoon platoon;
        // get the number of cars in this platoon
        platoon.size = int(platoonSize->doubleValue());
        // get the speed of this platoon
        platoon.speed = platoonInsertSpeed->doubleValue() / 3.6;
        // compute the distance of this platoon
        platoon.distanceToFront = platoonLeaderHeadway * platoon.speed;
        // compute the length of the platoon. assume a hardcoded vehicle length value of 4
        platoon.length = platoon.size * 4 + (platoon.size - 1) * (platoonInsertDistance + platoonInsertHeadway * platoon.speed);
        // add the length of this platoon to the platoons of this lane
        lengths[l] += platoon.length + platoon.distanceToFront;
        platoons.find(l)->second.push_back(platoon);
        // loop through all lanes
        l = (l + 1) % nLanes;
    }

    for(int l = 0; l < nLanes; l++){
        lengths[l] = 500 + (lengths[l]/2);
    }

    // finally inject vehicles
    double totalLength;
    for (l = 0; l < nLanes; l++) {
        totalLength = lengths[l];
        std::vector<Platoon> ps = platoons.find(l)->second;
        for (uint32_t p = 0; p < ps.size(); p++) {
            automated.id = vehTypeId;
            automated.speed = ps[p].speed;
            totalLength -= platoonLeaderHeadway * ps[p].speed;
            for (int v = 0; v < ps[p].size; v++) {
                automated.position = totalLength;
                automated.lane = l;
                addVehicleToQueue(0, automated);
                positions.addVehicleToPlatoon(injectedCars, v, injectedPlatoons);
                injectedCars++;
                if (v < ps[p].size - 1)
                    totalLength -= (4 + platoonInsertDistance + platoonInsertHeadway * automated.speed);
                else
                    totalLength -= 4;
            }
            injectedPlatoons++;
        }
    }
}

void JoinBDITrafficManager::injectJoiner(){
    struct Vehicle automated;
    automated.id = findVehicleTypeIndex(platooningVType);
    /*Insert the joiner vehicle*/
    if (joinerLane >= 0){
        for(int i = numJoiners; i > 0; i--){
            automated.speed = (platoonInsertSpeed->doubleValue() / 3.6);
            automated.lane = joinerLane;
            automated.position = 50 + (8*i);
            addVehicleToQueue(0, automated);
        }
    }
}
