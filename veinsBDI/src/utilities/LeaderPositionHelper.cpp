/*
 * LeaderPositionHelper.cpp
 *
 *  Created on: 17/09/2018
 *      Author: miguel
 */

#include "LeaderPositionHelper.h"


int LeaderPositionHelper::isPlatoonLeader(int id){
    int isInPlatoon = positions.vehToPlatoons.count(id);
    if(isInPlatoon == 0){
        return -1;
    }
    int platoonId = positions.vehToPlatoons[id];
    int position =  positions.positions[platoonId][id];

    if(position == 0){
        return platoonId;
    }
    return -1;
}
