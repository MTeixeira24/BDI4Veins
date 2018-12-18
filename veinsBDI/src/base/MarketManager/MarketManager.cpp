//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "MarketManager.h"
Define_Module(MarketManager);

void MarketManager::initialize(int stage){
    LightJasonManager::initialize(stage);
    if(stage == 0){

    }
}
void MarketManager::finish() {

}

std::vector<int> MarketManager::getElementsPreferredSpeed(const std::vector<int>& elementList){
    std::vector<int> preferredSpeeds(elementList.size());
    for(uint32_t i = 0; i < elementList.size(); i++){
        preferredSpeeds[i] = iterationSpeeds[elementList[i]];
    }
    return preferredSpeeds;
}

void MarketManager::parseResponse(uint32_t msgLength) {
    msgLength -= sizeof(uint32_t);
    LightJasonBuffer rbf = receiveMessage(msgLength);
    uint16_t type;
    uint16_t commandId;
    rbf >> commandId;
    ASSERT(commandId == QUERY || commandId == TERMINATE_CONNECTION);
    if(commandId == TERMINATE_CONNECTION){
        close(connSocket);
    }else{
        while(!rbf.eof()){
            uint32_t agentMessageLength; //TODO: Organize this into structs
            rbf >> agentMessageLength;
            agentMessageLength -= sizeof(uint32_t);
            uint32_t agentId;
            rbf >> agentId;
            uint32_t agentAction;
            rbf >> agentAction;
            switch (agentAction){
            default:
                break;
            }
        }
        queryMsg = new cMessage("query");
        scheduleAt(simTime() + updateInterval, queryMsg);
    }
}

void MarketManager::setLightJasonParameters() {
    LightJasonBuffer buff;
    jp.initializeParamsBuffer(buff);
    jp.setSimParameters(buff, par("agentManager").stdstringValue());
    jp.setSimParameters(buff, par("statisticsCollector").stdstringValue());
    jp.setSimParameters<int>(buff, par("platoon_size").intValue());
    jp.setSimParameters(buff, par("vote_rule").stdstringValue());
    jp.setSimParameters(buff, par("platoon_type").stdstringValue());
    jp.setSimParameters<int>(buff, par("iteration").intValue());
    jp.setSimParameters<double>(buff, par("factor").doubleValue());
    jp.setSimParameters<std::string>(buff, par("utilityFunction").stdstringValue());
    jp.setSimParameters<std::string>(buff, par("committee_vote_rule").stdstringValue());
    writeToSocket(buff.getBuffer());
}

