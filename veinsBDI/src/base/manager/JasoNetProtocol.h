/*
 * JasoNetProtocol.h
 *
 *  Created on: 07/08/2018
 *      Author: miguel
 */

#ifndef JASONETPROTOCOL_H_
#include <string>
#include <stdint.h>
#include <iostream>
#include "LightJasonConstants.h"
#include "LightJasonBuffer.h"
class JasoNetProtocol{
private:

public:
    JasoNetProtocol();
    ~JasoNetProtocol();

    LightJasonBuffer buildUpdateBeliefQuery(int id, std::string belief, double value); //TODO: Temp values for testing. Use the PLEXE:Vehicle struct
    LightJasonBuffer connectionRequest();
    LightJasonBuffer subscriptionRequest(uint32_t);
    LightJasonBuffer removeRequest(uint32_t);
    LightJasonBuffer terminateConnection();
    LightJasonBuffer query();
};



#endif /* JASONETPROTOCOL_H_ */
