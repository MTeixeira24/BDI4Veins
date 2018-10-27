/*
 * NoisePlatoonTrafficManager.h
 *
 *  Created on: Oct 12, 2018
 *      Author: miguel
 */

#ifndef TRAFFIC_NOISEPLATOONTRAFFICMANAGER_H_
#define TRAFFIC_NOISEPLATOONTRAFFICMANAGER_H_

#include "JoinBDITrafficManager.h"
#include <random>

class NoisePlatoonTrafficManager : public JoinBDITrafficManager {
public:
    NoisePlatoonTrafficManager() : suppressInjections(true), injectLane(0), baseSpeed(90) {};
    virtual ~NoisePlatoonTrafficManager(){};
    virtual void initialize(int stage) override;
    virtual void scenarioLoaded() override;
protected:
    virtual void handleSelfMsg(cMessage* msg) override;
    void injectVehicle(int lane, int speed);
    std::string noiseVType;
    int noiseLanes;
    //Prevent traffic injections from occurring at lanes occupied by platoons and joiners
    bool suppressInjections;
    //Current injection lane
    int injectLane;
    int baseSpeed;
    int vehTypeId;
    cMessage* addNoise;
};

#endif /* TRAFFIC_NOISEPLATOONTRAFFICMANAGER_H_ */
