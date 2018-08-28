/*
 * JasoNetProtocol.cpp
 *
 *  Created on: 07/08/2018
 *      Author: miguel
 */

#include "JasoNetProtocol.h"

JasoNetProtocol::JasoNetProtocol() {

}

JasoNetProtocol::~JasoNetProtocol(){

}

LightJasonBuffer JasoNetProtocol::buildUpdateBeliefQuery(int _id, std::string belief, double value){
    uint16_t size = 16+32+64;
    uint16_t action = SET_MAX_SPEED;
    uint32_t id = _id;
    double speed = value;
    return LightJasonBuffer() << size << action << id << speed;
}

LightJasonBuffer JasoNetProtocol::connectionRequest(){
    uint16_t size = 16;
    uint16_t action = CONNECTION_REQUEST;
    return LightJasonBuffer() << size << action;
}

LightJasonBuffer JasoNetProtocol::subscriptionRequest(uint32_t id, std::string vType, std::string aslFile){
    uint16_t size = 32;
    uint16_t action = ADD_AGENT;
    return LightJasonBuffer() << size << action << id << vType << aslFile;
}

LightJasonBuffer JasoNetProtocol::removeRequest(uint32_t id){
    uint16_t size = 32;
    uint16_t action = REMOVE_AGENT;
    return LightJasonBuffer() << size << action << id;
}

LightJasonBuffer JasoNetProtocol::terminateConnection(){
    uint16_t size = 16;
    uint16_t action = TERMINATE_CONNECTION;
    return LightJasonBuffer() << size << action;
}

LightJasonBuffer JasoNetProtocol::query(){
    uint16_t size = 16;
    uint16_t action = QUERY;
    return LightJasonBuffer() << size << action;
}
