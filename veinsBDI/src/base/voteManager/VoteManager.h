/*
 * VoteManager.h
 *
 *  Created on: 12/09/2018
 *      Author: miguel
 */

#ifndef BASE_VOTEMANAGER_VOTEMANAGER_H_
#define BASE_VOTEMANAGER_VOTEMANAGER_H_

#include "../manager/LightJasonManager.h"
#include "../../app/plexeappl/concrete/VotingAppl.h"

#include <unistd.h>
#include <thread>
#include "../../utilities/json.hpp"
#include <fstream>
using json = nlohmann::json;
class VoteManager : public LightJasonManager {
public:
    VoteManager(){};
    virtual ~VoteManager(){};
    void initialize(int) override;
    void finish() override;
    /**
     * Constants for timeStamp storage
     */
    enum class TimeStampAction{
        TIME_OF_VOTE_START,
        TIME_OF_VOTE_END,
        CHAIR_TO_JOINER_START,
        CHAIR_TO_JOINER_END,
        CHAIR_TO_MEMBER_END
     };
    /**
     * Store timestamps as requested by the controllers
     */
    void storeTimeStamp(double time, TimeStampAction action);
    /**
     * Get this agents associated preferred speed
     */
    int getPreferredSpeed(int agentId);
    /**
     * Get htis agents associated preferred path
     */
    std::vector<int> getPreferredPath(int agentId);
    /*
     * Get a list of speeds for a given array of ids
     */
    std::vector<int> getElementsPreferredSpeed(std::vector<int> elementList);

protected:
    void parseResponse(uint32_t msgLength) override;
private:
    /**
     * Store data about voting time
     */
    std::vector<double> chair2memberTimeStamps;
    std::vector<double> endOfVoteTimeStamps;
    double chair2joinerStart;
    double chair2joinerDelay;
    double startOfVoteTimeStamp;
    /**
     * Information about the preferred speeds for the current iteration
     */
    std::vector<int> iterationSpeeds;
    /**
     * Store the preferred paths of the agents
     */
    std::vector<std::vector<int>> preferredPaths;
};

#endif /* BASE_VOTEMANAGER_VOTEMANAGER_H_ */
