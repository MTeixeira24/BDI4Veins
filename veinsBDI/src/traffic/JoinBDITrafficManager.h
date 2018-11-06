/*
 * JoinBDITrafficManager.h
 *
 *  Created on: Sep 7, 2018
 *      Author: miguel
 */

#ifndef TRAFFIC_JOINBDITRAFFICMANAGER_H_
#define TRAFFIC_JOINBDITRAFFICMANAGER_H_

#include "veins/modules/mobility/traci/TraCIBaseTrafficManager.h"
#include "veins/modules/application/platooning/utilities/DynamicPositionManager.h"

class JoinBDITrafficManager: public TraCIBaseTrafficManager {
public:
    JoinBDITrafficManager()
        : platoonSize(0)
        , nPlatoons(0)
        , injectedCars(0)
        , injectedPlatoons(0)
        , positions(DynamicPositionManager::getInstance())
    {
        platoonInsertDistance = 0;
        platoonInsertHeadway = 0;
        platoonInsertSpeed = 0;
        platoonLeaderHeadway = 0;
        nLanes = 0;
    }
    ~JoinBDITrafficManager();

protected:
    virtual void initialize(int stage);
    virtual void scenarioLoaded();
    virtual void injectPlatoon();
    virtual void injectJoiner();
    int numJoiners;
    cPar* platoonSize;
    int nPlatoons;
    int injectedCars;
    int injectedPlatoons;
    DynamicPositionManager& positions;
    // number of lanes
    int nLanes;
    // insert distance
    double platoonInsertDistance;
    // insert headway
    double platoonInsertHeadway;
    // headway for leader vehicles
    double platoonLeaderHeadway;
    // sumo vehicle type of platooning cars
    std::string platooningVType;
    cPar* platoonInsertSpeed;
    int joinerLane;
    cMessage* addJoiner = NULL;

    typedef struct {
        int size;
        double speed;
        double length;
        double distanceToFront;
    } Platoon;

    virtual void handleSelfMsg(cMessage* msg) override;
};

#endif /* TRAFFIC_JOINBDITRAFFICMANAGER_H_ */
