/*
 * MergeManager.cpp
 *
 *  Created on: Feb 27, 2019
 *      Author: miguel
 */

#include "../../managers/MergeManager/MergeManager.h"
Define_Module(MergeManager);

MergeManager::MergeManager() {
}

MergeManager::~MergeManager() {
}

void MergeManager::initialize(int stage){
    LightJasonManager::initialize(stage);
    if(stage == 0){
        queryCountStats.setName("queryCount");
        responseTimes.setName("responseTimes");
    }
}

void MergeManager::finish() {
    LightJasonManager::finish();
}


void MergeManager::QueueTrigger(Trigger& trigger){
    queryCount++;
    LightJasonManager::QueueTrigger(trigger);
}
void MergeManager::sendTriggers(){
    startTime = std::chrono::steady_clock::now();
    LightJasonManager::sendTriggers();
}

void MergeManager::parseResponse(LightJasonBuffer rbf, uint32_t msgLength) {
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
            case SEND_OFFER:{
                assertType(rbf, VALUE_INT);
                int targetVehicleId = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int offer = extractData<int>(rbf);
                ((LaneMergeAgent*)(vehicles[jm.agentId]))->sendOffer(targetVehicleId, offer);
                break;
            }
            case SEND_DECISION:{
                assertType(rbf, VALUE_INT);
                int targetVehicleId = extractData<int>(rbf);
                assertType(rbf, VALUE_SHORT);
                short decision = extractData<short>(rbf);
                ((LaneMergeAgent*)(vehicles[jm.agentId]))->sendDecision(targetVehicleId, decision);
                break;
            }
            case SEND_PAYOUT:{
                assertType(rbf, VALUE_INT);
                int targetVehicleId = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int payout = extractData<int>(rbf);
                ((LaneMergeAgent*)(vehicles[jm.agentId]))->sendPayout(targetVehicleId, payout);
                break;
            }
            default:
                break;
            }
        }
        auto delta = std::chrono::steady_clock::now() - startTime;
        double ms = std::chrono::duration <double, std::milli> (delta).count();
        queryCountStats.record(queryCount);
        responseTimes.record(ms);
        queryCount = 0;
    }
}

void MergeManager::setLightJasonParameters() {
    LightJasonBuffer buff;
    jp.initializeParamsBuffer(buff);
    jp.setSimParameters(buff, par("agentManager").stdstringValue());
    jp.setSimParameters(buff, par("statisticsCollector").stdstringValue());
    jp.setSimParameters<int>(buff, par("iteration").intValue());
    writeToSocket(buff.getBuffer());
}

void MergeManager::assertType(LightJasonBuffer& buf, int value){
    uint16_t type;
    buf >> type;
    ASSERT(type == value);
}
