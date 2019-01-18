/*
 * MarketJoinTrafficManager.h
 *
 *  Created on: Jan 16, 2019
 *      Author: miguel
 */

#ifndef TRAFFIC_MARKETJOINTRAFFICMANAGER_H_
#define TRAFFIC_MARKETJOINTRAFFICMANAGER_H_

#include "JoinBDITrafficManager.h"

class MarketJoinTrafficManager : public JoinBDITrafficManager {
public:
    MarketJoinTrafficManager(){};
    virtual ~MarketJoinTrafficManager(){};
protected:
    virtual void injectPlatoon() override;
};

#endif /* TRAFFIC_MARKETJOINTRAFFICMANAGER_H_ */
