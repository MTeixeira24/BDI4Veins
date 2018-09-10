/*
 * JoinBDITrafficManager.cpp
 *
 *  Created on: Sep 7, 2018
 *  Based on: RingTrafficManager.cc
 *      Author: miguel
 */

#include "JoinBDITrafficManager.h"

Define_Module(JoinBDITrafficManager);

void JoinBDITrafficManager::initialize(int stage)
{

    TraCIBaseTrafficManager::initialize(stage);

    platoonSize = &par("platoonSize");
    nPlatoons = par("nPlatoons").intValue();
    nLanes = par("nLanes").intValue();
    platoonInsertSpeed = &par("platoonInsertSpeed");
    platoonInsertDistance = par("platoonInsertDistance").doubleValue();
    platoonInsertHeadway = par("platoonInsertHeadway").doubleValue();
    platoonLeaderHeadway = par("platoonLeaderHeadway").doubleValue();
    platooningVType = par("platooningVType").stdstringValue();
    joinerLane = par("joinerLane").intValue();
}

void JoinBDITrafficManager::scenarioLoaded()
{

    int vehTypeId = findVehicleTypeIndex("vtypeauto");
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
    /*Insert the joiner vehicle*/
    if (joinerLane >= 0){
        automated.speed = (platoonInsertSpeed->doubleValue() / 3.6) + 20;
        automated.lane = joinerLane;
        automated.position = 10;
        addVehicleToQueue(0, automated);
    }
}
