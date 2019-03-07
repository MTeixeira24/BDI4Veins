/*
 * LaneMergeAgent.h
 *
 *  Created on: Feb 27, 2019
 *      Author: miguel
 */

#ifndef APP_PLEXEAPPL_CONCRETE_LANEMERGE_LANEMERGEAGENT_H_
#define APP_PLEXEAPPL_CONCRETE_LANEMERGE_LANEMERGEAGENT_H_

#include <random>
#include <unordered_set>


#include "../../GeneralPlexeAgentAppl.h"
#include "../../../../messages/AckTimer_m.h"
#include "../../../../messages/utility/MessageCache.h"
#include "../../../../base/MergeManager/MergeManager.h"

class LaneMergeAgent : public GeneralPlexeAgentAppl {
public:
    LaneMergeAgent();
    virtual ~LaneMergeAgent();

    /** override from GeneralPlexeAgentAppl */
    virtual void initialize(int stage) override;
protected:
    virtual void handleLowerMsg(cMessage* msg) override;
    void handleSelfMsg(cMessage* msg) override;
    void resendMessage(long msgId, AckTimer* at);
    void sendMessageWithAck(MarketMessage* msg, const std::vector<int>& targets);
    void sendMessageWithAck(MarketMessage* msg, int target);
    void sendMessageDelayed(MarketMessage* msg, int target);
    /*
     * Check if this vehicle is the intended target of a message
     */
    virtual bool isReceiver(MarketMessage* msg);
    /*
     * Seconds to wait for ack messages
     */
    const double ackTime = 0.05;
private:
    /*
     * Manage the messages that are sent
     */
    MessageCache messageCache;
    /*
     * Timers
     */
    cMessage* debugTimer = NULL;
};

#endif /* APP_PLEXEAPPL_CONCRETE_LANEMERGE_LANEMERGEAGENT_H_ */
