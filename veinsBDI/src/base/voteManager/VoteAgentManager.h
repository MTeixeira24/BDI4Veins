/*
 * VoteAgentManager.h
 *
 *  Created on: 08/04/2019
 *      Author: miguel
 */

#ifndef BASE_VOTEMANAGER_VOTEAGENTMANAGER_H_
#define BASE_VOTEMANAGER_VOTEAGENTMANAGER_H_

#include "../manager/LightJasonManager.h"
#include "../../app/plexeappl/concrete/Vote/BaseVoteAppl.h"
#include <unistd.h>
#include <thread>
#include "../../utilities/json.hpp"
#include <fstream>
using json = nlohmann::json;
class VoteAgentManager : public LightJasonManager {
public:
    VoteAgentManager(){}
    virtual ~VoteAgentManager(){}
    void initialize(int stages) override;
    void finish() override;
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
    std::vector<int> getElementsPreferredSpeed(const std::vector<int>& elementList);
protected:
    void parseResponse(LightJasonBuffer rbf, uint32_t msgLength) override;
    /*
     * Define the parameters to send to the java agent manager
     */
    virtual void setLightJasonParameters() override;
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
private:
    /**
     * Store the preferred paths of the agents
     */
    std::vector<std::vector<int>> preferredPaths;
    /**
     * Information about the preferred speeds for the current iteration
     */
    std::vector<int> iterationSpeeds;
    void assertType(LightJasonBuffer& buf, int value);

    template <class T>
    T extractData(LightJasonBuffer& buf){
        T value;
        buf >> value;
        std::cout << value << " ";
        return value;
    }

    template <class T>
    T extractAndAssert(LightJasonBuffer& buf, int type){
        assertType(buf, type);
        return extractData<T>(buf);
    }
    inline BaseVoteAppl* voteVehicle(int agentId){return (BaseVoteAppl*)vehicles[agentId];};
};

#endif /* BASE_VOTEMANAGER_VOTEAGENTMANAGER_H_ */
