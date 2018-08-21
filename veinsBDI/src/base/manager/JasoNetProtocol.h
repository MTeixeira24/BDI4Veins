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
class JasoNetProtocol{
private:

public:
    JasoNetProtocol();
    ~JasoNetProtocol();
    std::string buildSubscriptionRequest(uint32_t id);
    std::string buildConnectionRequest();
    std::string buildBeliefUpdateRequest(uint32_t id, std::string belief, std::string value);
    /*Extracts the vehicle id and associated action from response and returns the associated data*/
    std::string parseResponse(std::string response, uint32_t* id, std::string & action);
    std::string buildRemoveRequest(int id);
};



#endif /* JASONETPROTOCOL_H_ */
