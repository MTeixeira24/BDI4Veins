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
};

#endif /* BASE_VOTEMANAGER_VOTEMANAGER_H_ */
