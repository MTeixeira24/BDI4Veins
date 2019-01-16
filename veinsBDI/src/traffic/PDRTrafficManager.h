/*
 * PDRTrafficManager.h
 *
 *  Created on: Oct 11, 2018
 *      Author: miguel
 */

#ifndef TRAFFIC_PDRTRAFFICMANAGER_H_
#define TRAFFIC_PDRTRAFFICMANAGER_H_

#include "veins/modules/mobility/traci/TraCIBaseTrafficManager.h"
#include <random>

class PDRTrafficManager : public TraCIBaseTrafficManager{
public:
    PDRTrafficManager() : separation(0){};
    virtual ~PDRTrafficManager(){};
    virtual void initialize(int stage);
    virtual void scenarioLoaded();
    virtual void handleSelfMsg(cMessage* msg) override;
private:
    int separation;
    std::string vehicleVType;
    std::string noiseVType;
    int flow;
    cMessage* injectTrafficTimer;
protected:
    void injectTraffic();
};

#endif /* TRAFFIC_PDRTRAFFICMANAGER_H_ */
