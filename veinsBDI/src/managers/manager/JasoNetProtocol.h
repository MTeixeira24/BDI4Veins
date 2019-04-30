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
#include "../../managers/manager/LightJasonConstants.h"
#include "../../managers/manager/LightJasonBuffer.h"
#include "../../managers/manager/BeliefModel.h"
namespace Jason{
class JasoNetProtocol{
private:
public:
    JasoNetProtocol();
    ~JasoNetProtocol();
    /*Build a request to update associated agents beliefs*/
    LightJasonBuffer buildAddGoalQuery(uint32_t, const void*);//(int id, std::string belief, double value); //TODO: Temp values for testing. Use the PLEXE:Vehicle struct
    /*Build a request to verify successful connection*/
    LightJasonBuffer connectionRequest();
    /*Build a request to create a new agent and associate it*/
    LightJasonBuffer subscriptionRequest(uint32_t, std::string, std::string);
    LightJasonBuffer initializeBulkSubscriptionRequest();
    void addToBulkSubscriptionRequest(LightJasonBuffer& buffer, uint32_t id,std::string vType, std::string aslFile);
    void terminateBulkSubscriptionRequest(LightJasonBuffer& buffer);
    /*Build a request to remove the specified agent*/
    LightJasonBuffer removeRequest(uint32_t);
    /*Build a request to terminate communications*/
    LightJasonBuffer terminateConnection();
    /*Build a request to query for decisions*/
    LightJasonBuffer query();
    /*Build a request to set simulation parameters*/
    LightJasonBuffer setSimParameters(std::string, std::string, int, int, double, std::string, std::string);

    void initializeParamsBuffer(LightJasonBuffer& buffer);

    template<typename t>
    void setSimParameters(LightJasonBuffer& buffer, t param){
        buffer << param;
    }
};

}

#endif /* JASONETPROTOCOL_H_ */
