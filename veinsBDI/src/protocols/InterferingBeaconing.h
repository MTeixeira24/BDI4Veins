/*
 * InterferingBeaconing.h
 *
 *  Created on: 18/10/2018
 *      Author: miguel
 */

#ifndef PROTOCOLS_INTERFERINGBEACONING_H_
#define PROTOCOLS_INTERFERINGBEACONING_H_

#include "veins/modules/application/platooning/protocols/SimplePlatooningBeaconing.h"

class InterferingBeaconing : public SimplePlatooningBeaconing {
public:
    InterferingBeaconing(){};
    virtual ~InterferingBeaconing(){};
    static const int INTERFEARING_TYPE = 32345;
    void sendPlatooningMessage(int destinationAddress);
};

#endif /* PROTOCOLS_INTERFERINGBEACONING_H_ */
