/*
 * JasoNetProtocol.cpp
 *
 *  Created on: 07/08/2018
 *      Author: miguel
 */

#include "JasoNetProtocol.h"

namespace Jason{

JasoNetProtocol::JasoNetProtocol() {

}

JasoNetProtocol::~JasoNetProtocol(){

}

LightJasonBuffer JasoNetProtocol::buildUpdateBeliefQuery(uint32_t id, const void* beliefModel){//(int _id, std::string belief, double value){
    BeliefModel* bm = (BeliefModel*) beliefModel;
    LightJasonBuffer buffer;
    uint32_t size = 6; //Minimum size is 2 bytes to represent the type of query and 4 bytes for agentID
    //calculate the size of the query
    size += bm->getTotalSize() + bm->getBelief().length();
    uint16_t action = SET_BELIEF;
    //First segment of query, size of message, action to perform(set belief), agentId,  and the belief string
    buffer << size <<  action << id << bm->getBelief();
    //Remainder segments: data type + data
    for(uint32_t i = 0; i < bm->getValues().size(); i++){
        BeliefObject bo = bm->getValues()[i];
        buffer << bo.getDataType();
        switch (bo.getDataType()){
        case VALUE_BOOL:
            throw cRuntimeError("Bool not implemented");
            break;
        case VALUE_CHAR:
            throw cRuntimeError("Char not implemented");
            break;
        case VALUE_SHORT:
            throw cRuntimeError("Short not implemented");
            break;
        case VALUE_INT:
            buffer << *((int*)bo.getData());
            break;
        case VALUE_LONG:
            throw cRuntimeError("Long not implemented");
            break;
        case VALUE_FLOAT:
            throw cRuntimeError("Float not implemented");
            break;
        case VALUE_DOUBLE:
            buffer << *((double*)bo.getData());
            break;
        default:
            throw cRuntimeError("Invalid data type!");
            break;
        }
    }

    return buffer;

    //uint32_t id = _id;
    //double speed = value;
    //return LightJasonBuffer() << size << action << id << speed;
}

LightJasonBuffer JasoNetProtocol::connectionRequest(){
    uint32_t size = 16;
    uint16_t action = CONNECTION_REQUEST;
    return LightJasonBuffer() << size << action;
}

LightJasonBuffer JasoNetProtocol::subscriptionRequest(uint32_t id, std::string vType, std::string aslFile){
    uint32_t size = 32;
    uint16_t action = ADD_AGENT;
    return LightJasonBuffer() << size << action << id << vType << aslFile;
}

LightJasonBuffer JasoNetProtocol::removeRequest(uint32_t id){
    uint32_t size = 32;
    uint16_t action = REMOVE_AGENT;
    return LightJasonBuffer() << size << action << id;
}

LightJasonBuffer JasoNetProtocol::terminateConnection(){
    uint32_t size = 16;
    uint16_t action = TERMINATE_CONNECTION;
    return LightJasonBuffer() << size << action;
}

LightJasonBuffer JasoNetProtocol::query(){
    uint32_t size = 16;
    uint16_t action = QUERY;
    return LightJasonBuffer() << size << action;
}

}
