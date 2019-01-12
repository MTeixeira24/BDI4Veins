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
        //Reserve size for timestamps
        endOfVoteTimeStamps.reserve(50);
        endOfRouteVoteTimeStamps.reserve(50);
        //Gather run information
        std::string platoonType = par("platoon_type").stdstringValue();
        int platoonSize = par("platoon_size").intValue();
        int iteration = par("iteration").intValue();
        //Gather information on agents preferences
        char buff[FILENAME_MAX];
        getcwd( buff, FILENAME_MAX );
        //Working directory is the simulations folder
        std::string current_working_dir(buff);
        json j;
        std::ifstream jsonFile ("preferedSpeeds.json");
        jsonFile >> j;
        iterationSpeeds.reserve(platoonSize);
        std::string paths = "Paths";
        for(uint32_t i = 0; i < j[platoonType][std::to_string(platoonSize)][std::to_string(iteration)].size(); i++){
            iterationSpeeds.push_back(j[platoonType][std::to_string(platoonSize)][std::to_string(iteration)][i].get<int>());
            std::vector<int> speeds;
            for(uint32_t z = 0; z < j[paths][std::to_string(platoonSize)][i].size(); z++){
                int aux = j[paths][std::to_string(platoonSize)][i][z].get<int>();
                speeds.push_back(aux);
            }
            preferredPaths.push_back(speeds);
        }


        json j2;
        std::ifstream jsonFile2 ("marketParameters.json");
        jsonFile2 >> j2;
        endowments.reserve(platoonSize + 5);
        wtp.reserve(platoonSize + 5);
        for(uint32_t i = 0; i < j2[std::to_string(platoonSize)][std::to_string(iteration)].size(); i++){
            endowments.push_back(j2[std::to_string(platoonSize)][std::to_string(iteration)][i]["endowment"].get<int>());
            wtp.push_back(j2[std::to_string(platoonSize)][std::to_string(iteration)][i]["wtp"].get<int>());
        }

        retransmissions = 0;
    }
}
void MarketManager::finish() {
    double maxTime;
    //Get the highest time
    if(endOfVoteTimeStamps.size() > 0){
        maxTime = endOfVoteTimeStamps[endOfVoteTimeStamps.size() - 1];
        double timeToConsensus = maxTime - startOfVoteTimeStamp;
        maxTime = endOfRouteVoteTimeStamps[endOfRouteVoteTimeStamps.size() - 1];
        double timeToRouteConsensus = maxTime - startOfRouteVoteTimeStamp;
        recordScalar("#timeToSpeedConsensus", timeToConsensus);
        recordScalar("#timeToRouteConsensus", timeToRouteConsensus);
        recordScalar("#retransmissions", retransmissions);
    }
}

void MarketManager::timeStampSpeed(double time){
    endOfVoteTimeStamps.push_back(time);
}

void MarketManager::timeStampRoute(double time){
    endOfRouteVoteTimeStamps.push_back(time);
}

void MarketManager::startTimeStampRoute(double time){
    startOfRouteVoteTimeStamp = time;
}


void MarketManager::startTimeStampSpeed(double time){
    startOfVoteTimeStamp = time;
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
            case SUBMIT_BID:{
                std::cout << "SUBMIT_BID ";
                assertType(rbf, VALUE_INT);
                int auctionId = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int context = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int bidValue = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int managerId = extractData<int>(rbf);
                ((MarketAgent*)(vehicles[jm.agentId]))->sendBid(auctionId,context,bidValue,managerId);
                std::cout << std::endl;
                break;
            }
            case SEND_AUCTION_RESULTS:{
                std::cout << "SEND_AUCTION_RESULTS ";
                assertType(rbf, VALUE_INT);
                int auctionId = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int auctionIteration = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int winnerId = extractData<int>(rbf);
                ((MarketAgent*)(vehicles[jm.agentId]))->sendAuctionResults(auctionId, auctionIteration, winnerId);
                break;
            }
            case NOTIFY_START_AUCTION:{
                std::cout << "NOTIFY_START_AUCTION ";
                assertType(rbf, VALUE_INT);
                int auctionId = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int context = extractData<int>(rbf);
                std::cout << std::endl;
                ((MarketAgent*)(vehicles[jm.agentId]))->sendNotificationOfAuction(auctionId, context);
                break;
            }
            case HANDLE_END_OF_AUCTION:{
                std::cout << "HANDLE_END_OF_AUCTION ";
                assertType(rbf, VALUE_INT);
                int auctionId = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int auctionIteration = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                uint32_t winnerId = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int pay = extractData<int>(rbf);
                if (winnerId == jm.agentId){
                    assertType(rbf, VALUE_INT);
                    int wtpSum = extractData<int>(rbf);
                    assertType(rbf, VALUE_INT);
                    int context = extractData<int>(rbf);
                    ((MarketAgent*)(vehicles[jm.agentId]))->handleEndOfAuction(auctionId, auctionIteration, winnerId, pay,
                            wtpSum, context);
                }else{
                    ((MarketAgent*)(vehicles[jm.agentId]))->handleEndOfAuction(auctionId, auctionIteration, winnerId, pay);
                }
                std::cout << std::endl;
                break;
            }
            case SEND_PAY:{
                std::cout << "SEND_PAY ";
                assertType(rbf, VALUE_INT);
                int auctionId = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int context = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int pay = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int managerId = extractData<int>(rbf);
                rbf >> jm.type;
                if(jm.type == VALUE_ARRAY){
                    std::vector<int> route = parseArrayMessage(rbf);
                    ((MarketAgent*)(vehicles[jm.agentId]))->sendPay(auctionId, context, pay, managerId, route);
                }else if(jm.type == VALUE_INT){
                    int speed = extractData<int>(rbf);
                    ((MarketAgent*)(vehicles[jm.agentId]))->sendPay(auctionId, context, pay, managerId, speed);
                }
                std::cout << std::endl;
                break;
            }
            case DISTRIBUTE_PAY:{
                std::cout << "DISTRIBUTE_PAY ";
                assertType(rbf, VALUE_INT);
                int auctionId = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int auctionIteration = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int winnerId = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int payment = extractData<int>(rbf);
                assertType(rbf, VALUE_INT);
                int wtpSum = extractData<int>(rbf);
                rbf >> jm.type;
                if(jm.type == VALUE_ARRAY){
                    std::vector<int> route = parseArrayMessage(rbf);
                    ((MarketAgent*)(vehicles[jm.agentId]))->distributePay(auctionId, auctionIteration, winnerId, payment, wtpSum, route);
                }else if(jm.type == VALUE_INT){
                    int speed = extractData<int>(rbf);
                    ((MarketAgent*)(vehicles[jm.agentId]))->distributePay(auctionId, auctionIteration, winnerId, payment, wtpSum, speed);
                }
                std::cout << std::endl;
                break;
            }
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
    jp.setSimParameters(buff, par("platoon_type").stdstringValue());
    jp.setSimParameters<int>(buff, par("iteration").intValue());
    jp.setSimParameters<double>(buff, par("factor").doubleValue());
    jp.setSimParameters<std::string>(buff, par("utilityFunction").stdstringValue());
    jp.setSimParameters<std::string>(buff, par("auctionModule").stdstringValue());
    writeToSocket(buff.getBuffer());
}

void MarketManager::assertType(LightJasonBuffer& buf, int value){
    uint16_t type;
    buf >> type;
    ASSERT(type == value);
}
