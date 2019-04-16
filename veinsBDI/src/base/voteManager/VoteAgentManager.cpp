/*
 * VoteAgentManager.cpp
 *
 *  Created on: 08/04/2019
 *      Author: miguel
 */

#include "VoteAgentManager.h"

Define_Module(VoteAgentManager);

void VoteAgentManager::initialize(int stage){
    LightJasonManager::initialize(stage);
    if(stage == 0){
        queryCountStats.setName("queryCount");
        responseTimes.setName("responseTimes");

        std::string platoonType = par("platoon_type").stdstringValue();
        int platoonSize = par("platoon_size").intValue();
        int iteration = par("iteration").intValue();

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
    }

}

void VoteAgentManager::finish(){
    LightJasonManager::finish();
}

void VoteAgentManager::QueueTrigger(Trigger& trigger){
    queryCount++;
    LightJasonManager::QueueTrigger(trigger);
}
void VoteAgentManager::sendTriggers(){
    startTime = std::chrono::steady_clock::now();
    LightJasonManager::sendTriggers();
}


void VoteAgentManager::setLightJasonParameters(){
    //Collect information about the current run
    LightJasonBuffer buff;
    jp.initializeParamsBuffer(buff);
    jp.setSimParameters(buff, par("agentManager").stdstringValue());
    jp.setSimParameters<int>(buff, par("platoon_size").intValue());
    jp.setSimParameters(buff, par("vote_rule").stdstringValue());
    jp.setSimParameters<std::string>(buff, par("committee_vote_rule").stdstringValue());
    jp.setSimParameters<int>(buff, par("background_density").intValue());
    int x = par("iteration").intValue();
    jp.setSimParameters<int>(buff, par("iteration").intValue());


    jp.setSimParameters<double>(buff, par("factor").doubleValue());
    jp.setSimParameters<std::string>(buff, par("utilityFunction").stdstringValue());

//    jp.setSimParameters(buff, par("statisticsCollector").stdstringValue());
//    jp.setSimParameters<int>(buff, par("platoon_size").intValue());
//    jp.setSimParameters(buff, par("vote_rule").stdstringValue());
//    jp.setSimParameters(buff, par("platoon_type").stdstringValue());
//    jp.setSimParameters<int>(buff, par("iteration").intValue());
//    jp.setSimParameters<double>(buff, par("factor").doubleValue());
//    jp.setSimParameters<std::string>(buff, par("utilityFunction").stdstringValue());
//    jp.setSimParameters<std::string>(buff, par("committee_vote_rule").stdstringValue());

    writeToSocket(buff.getBuffer());
}

std::vector<int> VoteAgentManager::getElementsPreferredSpeed(const std::vector<int>& elementList){
    std::vector<int> preferredSpeeds(elementList.size());
    for(uint32_t i = 0; i < elementList.size(); i++){
        preferredSpeeds[i] = iterationSpeeds[elementList[i]];
    }
    return preferredSpeeds;
}

std::vector<int> VoteAgentManager::getPreferredPath(int agentId){
    return preferredPaths[agentId];
}

int VoteAgentManager::getPreferredSpeed(int agentId){
    return iterationSpeeds[agentId];
}


void VoteAgentManager::assertType(LightJasonBuffer& buf, int value){
    uint16_t type;
    buf >> type;
    ASSERT(type == value);
}

void VoteAgentManager::parseResponse(LightJasonBuffer rbf, uint32_t msgLength) {
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
            case NOTIFY_START_VOTE:{
                /*get the context identifier*/
                int contextId = extractAndAssert<int>(rbf, VALUE_INT);
                /*get the context arguments*/
                rbf >> jm.type;
                /*Is it a type specifier? If so check the following short to assert that it saying that no context is present*/
                if(jm.type == VALUE_SHORT){
                    rbf >> jm.type;
                    ASSERT(jm.type == VALUE_NULL);
                }else{
                    /*Otherwise assert that we have a received a context array*/
                    ASSERT(jm.type == VALUE_ARRAY);
                }
                std::vector<double> contextArgs;
                if(jm.type == VALUE_ARRAY)
                    contextArgs = parseDoubleArrayMessage(rbf);
                /*get the candidates array*/
                rbf >> jm.type;
                ASSERT(jm.type == VALUE_ARRAY);
                std::vector<int> candidates = parseArrayMessage(rbf);
                /*To assure consistency, get the size of the vote vectors that will be received*/
                int expectedVoteVector = extractAndAssert<int>(rbf, VALUE_INT);
                voteVehicle(jm.agentId)->sendNotificationOfVoteGeneral(contextId, contextArgs, candidates, expectedVoteVector);
                break;
            }
            case SUBMIT_VOTE:{
                rbf >> jm.type;
                ASSERT(jm.type == VALUE_ARRAY);
                std::vector<int> votes = parseArrayMessage(rbf);
                //Save the votes into controller
                voteVehicle(jm.agentId)->sendVoteSubmition(votes);
                break;
            }
            case SEND_VOTE_RESULTS:{
                //If the negotiation involves external participants.
                int externalId = extractAndAssert<int>(rbf, VALUE_INT);
                rbf >> jm.type;
                //Must be either an int for single election or an array for committees
                ASSERT(jm.type == VALUE_INT || jm.type == VALUE_ARRAY);
                if(jm.type == VALUE_INT){
                    int winnerValue = extractData<int>(rbf);
                    voteVehicle(jm.agentId)->sendVoteResults(winnerValue, externalId);
                }else{
                    std::vector<int> results = parseArrayMessage(rbf);
                    voteVehicle(jm.agentId)->sendCommitteeVoteResults(results);
                }
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

