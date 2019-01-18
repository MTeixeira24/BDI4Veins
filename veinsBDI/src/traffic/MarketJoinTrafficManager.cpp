/*
 * MarketJoinTrafficManager.cpp
 *
 *  Created on: Jan 16, 2019
 *      Author: miguel
 */

#include "MarketJoinTrafficManager.h"

Define_Module(MarketJoinTrafficManager)

void MarketJoinTrafficManager::injectPlatoon(){
    int vehTypeId = findVehicleTypeIndex(platooningVType);
    struct Vehicle automated;
    int joinerGroupSize = 3;

    // map from lane index to a vector with all platoon sizes
    std::map<int, std::vector<Platoon>> platoons;
    // total vehicle length for each lane
    std::vector<double> lengths;

    for (int l = 0; l < nLanes; l++) {
        platoons[l] = std::vector<Platoon>();
        lengths.push_back(0);
    }

    int l = 0;
    Platoon platoon1;
    // get the number of cars in this platoon
    platoon1.size = int(platoonSize->doubleValue());
    // get the speed of this platoon
    platoon1.speed = platoonInsertSpeed->doubleValue() / 3.6;
    // compute the distance of this platoon
    platoon1.distanceToFront = platoonLeaderHeadway * platoon1.speed;
    // compute the length of the platoon. assume a hardcoded vehicle length value of 4
    platoon1.length = platoon1.size * 4 + (platoon1.size - 1) * (platoonInsertDistance + platoonInsertHeadway * platoon1.speed);
    // add the length of this platoon to the platoons of this lane
    lengths[l] += platoon1.length + platoon1.distanceToFront;
    platoons.find(l)->second.push_back(platoon1);
    // loop through all lanes
    l = (l + 1) % nLanes;

    Platoon platoon2;
    // get the number of cars in this platoon
    platoon2.size = joinerGroupSize;
    // get the speed of this platoon
    platoon2.speed = platoonInsertSpeed->doubleValue() / 3.6;
    // compute the distance of this platoon
    platoon2.distanceToFront = platoonLeaderHeadway * platoon2.speed;
    // compute the length of the platoon. assume a hardcoded vehicle length value of 4
    platoon2.length = platoon2.size * 4 + (platoon2.size - 1) * (platoonInsertDistance + platoonInsertHeadway * platoon2.speed);
    // add the length of this platoon to the platoons of this lane
    lengths[l] += platoon2.length + platoon2.distanceToFront;
    platoons.find(l)->second.push_back(platoon2);
    // loop through all lanes
    l = (l + 1) % nLanes;

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
