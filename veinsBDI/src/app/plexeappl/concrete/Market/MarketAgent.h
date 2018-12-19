/*
 * MarketAgent.h
 *
 *  Created on: Dec 18, 2018
 *      Author: miguel
 */

#ifndef APP_PLEXEAPPL_CONCRETE_MARKET_MARKETAGENT_H_
#define APP_PLEXEAPPL_CONCRETE_MARKET_MARKETAGENT_H_

#include <random>
#include <unordered_set>

#include "veins/modules/application/platooning/utilities/DynamicPositionHelper.h"

#include "../../GeneralPlexeAgentAppl.h"
#include "../../../../utilities/LeaderPositionHelper.h"
#include "../../../../messages/AckTimer_m.h"
#include "../../../../messages/utility/MessageCache.h"


class MarketAgent : public GeneralPlexeAgentAppl {
public:
    MarketAgent(){};
    virtual ~MarketAgent();

    /** override from GeneralPlexeAgentAppl */
    virtual void initialize(int stage) override;
protected:
    /**
     *
     */
    virtual void delegateNegotiationMessage(NegotiationMessage* nm);
    /**
     * Extend from GeneralPlexeAgentAppl to handle messages related to voting
     */
    virtual void handleLowerMsg(cMessage* msg) override;
    /**
     * Fill a negotiation message with the standard information
     */
    void fillNegotiationMessage(MarketMessage* msg, int originId, int targetId,
            bool forWholePlatoon=true);
    void fillNegotiationMessage(MarketMessage* msg, int originId, int targetId,
            bool forWholePlatoon, std::unordered_set<int>& targets);
    /**
     * Handle messages sent to self
     */
    void handleSelfMsg(cMessage* msg) override;
    /**
     * Resends a message from cache
     */
    void resendMessage(long msgId);
private:
    /*
     * Manage the messages that are sent
     */
    MessageCache messageCache;
};

#endif /* APP_PLEXEAPPL_CONCRETE_MARKET_MARKETAGENT_H_ */
