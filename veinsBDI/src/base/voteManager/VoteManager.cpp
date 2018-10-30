/*
 * VoteManager.cpp
 *
 *  Created on: 12/09/2018
 *      Author: miguel
 */

#include "VoteManager.h"

Define_Module(VoteManager);

void VoteManager::finish(){
    double maxTime = -1;
    //Get the highest time
    if(endOfVoteTimeStamps.size() > 0){
        for(int i = endOfVoteTimeStamps.size() - 1; i >= 0; i--){
            if(endOfVoteTimeStamps[i] > maxTime) maxTime = endOfVoteTimeStamps[i];
        }
        double timeToConsensus = maxTime - startOfVoteTimeStamp;
        maxTime = -1;
        for(int i = chair2memberTimeStamps.size() - 1; i >= 0; i--){
            if(chair2memberTimeStamps[i] > maxTime) maxTime = chair2memberTimeStamps[i];
        }
        double highestDelayToMember = maxTime - startOfVoteTimeStamp;
        recordScalar("#timeToConsensus", timeToConsensus);
        recordScalar("#highestDelayToMember", highestDelayToMember);
        recordScalar("#delayToJoiner", chair2joinerDelay);
        recordScalar("#retransmissions", voteRetransmissions);
    }
}

void VoteManager::initialize(int stage){
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    LightJasonManager::initialize(stage);
    if(stage == 0){

        //Collect information about the current run
        std::string platoonType = par("platoon_type").stdstringValue();
        int platoonSize = par("platoon_size").intValue();
        int iteration = par("iteration").intValue();

        writeToSocket(jp.setSimParameters(par("vote_rule").stdstringValue(), platoonType, platoonSize,
                iteration, par("factor").doubleValue(), par("utilityFunction").stdstringValue(),
                par("committee_vote_rule").stdstringValue()).getBuffer());

        char buff[FILENAME_MAX];
        getcwd( buff, FILENAME_MAX );
        //Working directory is the simulations folder
        std::string current_working_dir(buff);

        json j;
        std::ifstream jsonFile ("preferedSpeeds.json");
        jsonFile >> j;
        iterationSpeeds.reserve(platoonSize);
        std::string paths = "Paths";
        for(int i = 0; i < platoonSize + 3; i++){
            iterationSpeeds.push_back(j[platoonType][std::to_string(platoonSize)][std::to_string(iteration)][i].get<int>());
            std::vector<int> speeds;
            for(int z = 0; z < j[paths][std::to_string(platoonSize)][i].size(); z++){
                int aux = j[paths][std::to_string(platoonSize)][i][z].get<int>();
                speeds.push_back(aux);
            }
            preferredPaths.push_back(speeds);
        }
        voteRetransmissions = 0;
    }

}

void VoteManager::incrementRetransmission(){
    voteRetransmissions++;
}

int VoteManager::getPreferredSpeed(int agentId){
    return iterationSpeeds[agentId];
}

std::vector<int> VoteManager::getElementsPreferredSpeed(std::vector<int> elementList){
    std::vector<int> preferredSpeeds(elementList.size());
    for(int i = 0; i < elementList.size(); i++){
        preferredSpeeds[i] = iterationSpeeds[elementList[i]];
    }
    return preferredSpeeds;
}

std::vector<int> VoteManager::getPreferredPath(int agentId){
    return preferredPaths[agentId];
}

void VoteManager::storeTimeStamp(double time, TimeStampAction action){
    switch(action){
    case TimeStampAction::TIME_OF_VOTE_START:{
        //Called by sendNotificationOfVote
        startOfVoteTimeStamp = time;
        break;
    }
    case TimeStampAction::TIME_OF_VOTE_END:{
        //Called by handleNotificationOfResults
        endOfVoteTimeStamps.push_back(time);
        break;
    }
    case TimeStampAction::CHAIR_TO_JOINER_START:{
        //Called by sendVoteResults
        chair2joinerStart = time;
        break;
    }
    case TimeStampAction::CHAIR_TO_JOINER_END:{
        //Called by handleNotificationOfResults
        chair2joinerDelay = time - chair2joinerStart;
        break;
    }
    case TimeStampAction::CHAIR_TO_MEMBER_END:{
        //Called by handleNotifyVote
        chair2memberTimeStamps.push_back(time);
        break;
    }
    default:
        break;
    }
}

void VoteManager::parseResponse(uint32_t msgLength){
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
            int platoonid;
            int leaderid;
            double preferedspeed;
            double tolerance;
            switch (agentAction){
            case SEND_JOIN_REQUEST:
            {
                rbf >> type;
                ASSERT(type == VALUE_INT);
                rbf >> platoonid;
                rbf >> type;
                ASSERT(type == VALUE_INT);
                rbf >> leaderid;
                rbf >> type;
                ASSERT(type == VALUE_DOUBLE);
                rbf >> preferedspeed;
                rbf >> type;
                ASSERT(type == VALUE_DOUBLE);
                rbf >> tolerance;
                ((VotingAppl*)(vehicles[agentId]))->sendRequestToJoin(platoonid, leaderid, preferedspeed, tolerance);
                break;
            }
            case SUBMIT_VOTE:
            {
                rbf >> type;
                ASSERT(type == VALUE_ARRAY);
                std::vector<int> votes = parseArrayMessage(rbf);
                //Save the votes into controller
                ((VotingAppl*)(vehicles[agentId]))->setVote(votes);
                ((VotingAppl*)(vehicles[agentId]))->sendVoteSubmition(votes);
                break;
            }
            case SEND_VOTE_RESULTS:
            {
                int externalId; //If the negotiation involves external participants.
                rbf >> type;
                rbf >> externalId;
                rbf >> type;
                //Must be either an int for single election or an array for committees
                ASSERT(type == VALUE_INT || type == VALUE_ARRAY);
                if(type == VALUE_INT){
                    int winnerValue;
                    rbf >> winnerValue;
                    ((VotingAppl*)(vehicles[agentId]))->sendVoteResults(winnerValue, externalId);
                }else{
                    std::vector<int> results = parseArrayMessage(rbf);
                    ((VotingAppl*)(vehicles[agentId]))->sendCommitteeVoteResults(results);
                }
                break;
            }
            case NOTIFY_START_VOTE:
            {
                /*get the context identifier*/
                rbf >> type;
                ASSERT(type == VALUE_INT);
                int contextId;
                rbf >> contextId;
                /*get the context arguments*/
                rbf >> type;
                /*Is it a type specifier? If so check the following short to assert that it saying that no context is present*/
                if(type == VALUE_SHORT){
                    rbf >> type;
                    ASSERT(type == VALUE_NULL);
                }else{
                    /*Otherwise assert that we have a received a context array*/
                    ASSERT(type == VALUE_ARRAY);
                }
                std::vector<double> contextArgs;
                if(type == VALUE_ARRAY){
                    contextArgs = parseDoubleArrayMessage(rbf);
                }
                /*get the candidates array*/
                rbf >> type;
                ASSERT(type == VALUE_ARRAY);
                std::vector<int> candidates = parseArrayMessage(rbf);
                /*To assure consistency, get the size of the vote vectors that will be received*/
                rbf >> type;
                ASSERT(type == VALUE_INT);
                int expectedVoteVector;
                rbf >> expectedVoteVector;
                ((VotingAppl*)(vehicles[agentId]))->sendNotificationOfVoteGeneral(contextId, contextArgs, candidates, expectedVoteVector);
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
