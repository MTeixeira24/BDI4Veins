/*
 * MergeManager.cpp
 *
 *  Created on: Feb 27, 2019
 *      Author: miguel
 */

#include "MergeManager.h"
Define_Module(MergeManager);

MergeManager::MergeManager() {

}

MergeManager::~MergeManager() {
}

void MergeManager::initialize(int stage){
    LightJasonManager::initialize(stage);
    if(stage == 0){
    }
}

void MergeManager::finish() {
}

void MergeManager::parseResponse(uint32_t msgLength) {
    msgLength -= sizeof(uint32_t);
    LightJasonBuffer rbf = receiveMessage(msgLength);
    JasonMessage jm;
    rbf >> jm.commandId;
    ASSERT(jm.commandId == QUERY || jm.commandId == TERMINATE_CONNECTION);
    if(jm.commandId == TERMINATE_CONNECTION){
        close(connSocket);
    }else{
        while(!rbf.eof()){
            rbf >> jm.agentMessageLength;
            jm.agentMessageLength -= sizeof(uint32_t);
            rbf >> jm.agentId;
            rbf >> jm.agentAction;
            switch (jm.agentAction){
            default:
                break;
            }
        }
        queryMsg = new cMessage("query");
        scheduleAt(simTime() + updateInterval, queryMsg);
    }
}

void MergeManager::setLightJasonParameters() {
    LightJasonBuffer buff;
    jp.initializeParamsBuffer(buff);
    /*jp.setSimParameters(buff, par("agentManager").stdstringValue());
    jp.setSimParameters(buff, par("statisticsCollector").stdstringValue());
    jp.setSimParameters<int>(buff, par("platoon_size").intValue());
    jp.setSimParameters(buff, par("platoon_type").stdstringValue());
    jp.setSimParameters<int>(buff, par("iteration").intValue());
    jp.setSimParameters<double>(buff, par("factor").doubleValue());
    jp.setSimParameters<std::string>(buff, par("utilityFunction").stdstringValue());
    jp.setSimParameters<std::string>(buff, par("auctionModule").stdstringValue());*/
    writeToSocket(buff.getBuffer());
}

void MergeManager::assertType(LightJasonBuffer& buf, int value){
    uint16_t type;
    buf >> type;
    ASSERT(type == value);
}
