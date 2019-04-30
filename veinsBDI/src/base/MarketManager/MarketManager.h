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

#ifndef BASE_MARKETMANAGER_MARKETMANAGER_H_
#define BASE_MARKETMANAGER_MARKETMANAGER_H_

#include "../manager/LightJasonManager.h"
#include "../../app/Market/MarketAgent.h"
#include "../manager/constants/MarketConstants.h"

#include <unistd.h>
#include "../../utilities/json.hpp"
#include <fstream>
using json = nlohmann::json;

class MarketManager : public LightJasonManager {
public:
    MarketManager(){};
    virtual ~MarketManager(){};
    void initialize(int) override;
    void finish() override;
    /**
     * Get this agents associated preferred speed
     */
    int getPreferredSpeed(int agentId){return iterationSpeeds[agentId];}
    /**
     * Get htis agents associated preferred path
     */
    std::vector<int> getPreferredPath(int agentId){return preferredPaths[agentId];}
    /*
     * Get a list of speeds for a given array of ids
     */
    std::vector<int> getElementsPreferredSpeed(const std::vector<int>& elementList);
    /*
     * Increments the retransmission counts
     */
    /*
     *
     */
    int getEndowment(int agentId){return endowments[agentId];}
    /*
     *
     */
    int getWTP(int agentId){return wtp[agentId];}
    void incrementRetransmission(){retransmissions++;}
    /**
     * Data structure to hold information from lightjason messages
     */
    struct JasonMessage{
        uint16_t commandId;
        uint16_t type;
        uint32_t agentMessageLength;
        uint32_t agentId;
        uint32_t agentAction;
    };
    void timeStampSpeed(double time);
    void timeStampRoute(double time);
    void startTimeStampRoute(double time);
    void startTimeStampSpeed(double time);
    void startTimeStampJoin(double time){startJoinTimeStamp = time;}
    void endTimeStampJoin(double time){endJoinTimeStamp = time;}

protected:
    void parseResponse(uint32_t msgLength) override;
    /*
     * Define the parameters to send to the java agent manager
     */
    virtual void setLightJasonParameters() override;

    void assertType(LightJasonBuffer& buf, int value);

    template <class T>
    T extractData(LightJasonBuffer& buf){
        T value;
        buf >> value;
        std::cout << value << " ";
        return value;
    }
private:
    /**
     * Timestamps
     */
    std::vector<int> endOfVoteTimeStamps;
    std::vector<int> endOfRouteVoteTimeStamps;
    double endJoinTimeStamp = 0;
    double startJoinTimeStamp = 0;
    double startOfRouteVoteTimeStamp;
    double startOfVoteTimeStamp;
    /**
     * Retransmissions
     */
    int retransmissions;
    /**
     * Information about the preferred speeds for the current iteration
     */
    std::vector<int> iterationSpeeds;
    /**
     * Store the preferred paths of the agents
     */
    std::vector<std::vector<int>> preferredPaths;
    /**
     *
     */
    std::vector<int> endowments;
    std::vector<int> wtp;
};

#endif /* BASE_MARKETMANAGER_MARKETMANAGER_H_ */
