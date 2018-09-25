/*
 * LeaderPositionHelper.h
 *
 *  Created on: 17/09/2018
 *      Author: miguel
 */

#ifndef UTILITIES_LEADERPOSITIONHELPER_H_
#define UTILITIES_LEADERPOSITIONHELPER_H_

#include "veins/modules/application/platooning/utilities/DynamicPositionHelper.h"

/**
 * Implements a position helper to determine leadership status of a given vehicle
 */
class LeaderPositionHelper : public DynamicPositionHelper {
public:
    LeaderPositionHelper(){};
    virtual ~LeaderPositionHelper(){};
    int isPlatoonLeader(int id);
};

#endif /* UTILITIES_LEADERPOSITIONHELPER_H_ */
