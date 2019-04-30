/*
 * MergeManager.h
 *
 *  Created on: Feb 27, 2019
 *      Author: miguel
 */

#ifndef MANAGERS_MERGEMANAGER_MERGEMANAGER_H_
#define MANAGERS_MERGEMANAGER_MERGEMANAGER_H_

#include "../manager/LightJasonManager.h"
#include "../../app/laneMerge/LaneMergeAgent.h"

#include <unistd.h>
#include "../../utilities/json.hpp"
#include <fstream>

#include "../../managers/manager/constants/BargainConstants.h"
using json = nlohmann::json;

class MergeManager : public LightJasonManager {
public:
    MergeManager();
    virtual ~MergeManager();
    void initialize(int) override;
    void finish() override;
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
protected:
    void parseResponse(LightJasonBuffer rbf, uint32_t msgLength) override;
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
    //Capture calls to queue and send trigger for statistical purposes
    cOutVector queryCountStats;
    cOutVector responseTimes;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    uint32_t queryCount = 0;
    void QueueTrigger(Trigger& trigger) override;
    void sendTriggers() override;
};

#endif /* MANAGERS_MERGEMANAGER_MERGEMANAGER_H_ */
