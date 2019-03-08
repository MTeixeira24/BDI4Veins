/*
 * LaneTrafficManager.h
 *
 *  Created on: 08/03/2019
 *      Author: miguel
 */

#ifndef TRAFFIC_LANETRAFFICMANAGER_H_
#define TRAFFIC_LANETRAFFICMANAGER_H_

#include "veins/modules/mobility/traci/TraCIBaseTrafficManager.h"

class LaneTrafficManager: public TraCIBaseTrafficManager  {
public:
    LaneTrafficManager()
        : continuousInjection(false)
        , vPairs(1)
        , lanePairs(1)
        , injectionTimeHeadway(4)
        , travelSpeed(80)
    {
        injectTimer = NULL;
    };
    virtual ~LaneTrafficManager();
protected:

    bool continuousInjection;
    int vPairs;
    int lanePairs;
    int injectionTimeHeadway;
    int travelSpeed;
    std::string vType;
    cMessage *injectTimer;
    virtual void initialize(int stage) override;
    virtual void scenarioLoaded() override;
    virtual void injectPair();
    virtual void injectInitialPairs();
    virtual void handleSelfMsg(cMessage* msg) override;
};

#endif /* TRAFFIC_LANETRAFFICMANAGER_H_ */
