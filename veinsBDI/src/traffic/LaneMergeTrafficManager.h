/*
 * LaneMergeTrafficManager.h
 *
 *  Created on: 21/08/2018
 *      Author: miguel
 */

#ifndef TRAFFIC_LANEMERGETRAFFICMANAGER_H_
#define TRAFFIC_LANEMERGETRAFFICMANAGER_H_

#include <veins/modules/application/platooning/traffic/PlatoonsTrafficManager.h>

class LaneMergeTrafficManager : public PlatoonsTrafficManager {
public:
    virtual void initialize(int stage);
    LaneMergeTrafficManager() : PlatoonsTrafficManager(){
        insertArterialMessage = 0;
    }
    virtual ~LaneMergeTrafficManager();
protected:
    std::string mergerVtype;
    cMessage* insertArterialMessage;
    struct Vehicle merger;
    void insertArterial();

    virtual void handleSelfMsg(cMessage* msg) override;
};
#endif /* TRAFFIC_LANEMERGETRAFFICMANAGER_H_ */
