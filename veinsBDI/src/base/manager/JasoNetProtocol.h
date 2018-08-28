/*
 * JasoNetProtocol.h
 *
 *  Created on: 07/08/2018
 *      Author: miguel
 */

/*
 * Builds messages for communication with the LightJason server
 */

#ifndef JASONETPROTOCOL_H_
#include <string>
#include <stdint.h>
#include <iostream>
#include "LightJasonConstants.h"
#include "LightJasonBuffer.h"
using namespace Jason;
class JasoNetProtocol{
private:

public:
    JasoNetProtocol();
    ~JasoNetProtocol();
    /*Build a request to update associated agents beliefs*/
    LightJasonBuffer buildUpdateBeliefQuery(int id, std::string belief, double value); //TODO: Temp values for testing. Use the PLEXE:Vehicle struct
    /*Build a request to verify successful connection*/
    LightJasonBuffer connectionRequest();
    /*Build a request to create a new agent and associate it*/
    LightJasonBuffer subscriptionRequest(uint32_t, std::string);
    /*Build a request to remove the specified agent*/
    LightJasonBuffer removeRequest(uint32_t);
    /*Build a request to terminate communications*/
    LightJasonBuffer terminateConnection();
    /*Build a request to query for decisions*/
    LightJasonBuffer query();
};



#endif /* JASONETPROTOCOL_H_ */
