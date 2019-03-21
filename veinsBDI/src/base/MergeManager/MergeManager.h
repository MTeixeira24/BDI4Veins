/*
 * MergeManager.h
 *
 *  Created on: Feb 27, 2019
 *      Author: miguel
 */

#ifndef BASE_MERGEMANAGER_MERGEMANAGER_H_
#define BASE_MERGEMANAGER_MERGEMANAGER_H_

#include "../manager/LightJasonManager.h"
#include "../manager/constants/BargainConstants.h"
#include "../../app/plexeappl/concrete/laneMerge/LaneMergeAgent.h"

#include <unistd.h>
#include "../../utilities/json.hpp"
#include <fstream>
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
};

#endif /* BASE_MERGEMANAGER_MERGEMANAGER_H_ */
