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
    MarketAgent(){
        messageCache.setSenderId(myId);
    };
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
    void fillNegotiationMessage(MarketMessage* msg, int originId, int targetId,
            bool forWholePlatoon, int idOfOriginMessage);
    /**
     * Handle messages sent to self
     */
    void handleSelfMsg(cMessage* msg) override;
    /**
     * Resends a message from cache
     */
    void resendMessage(long msgId, AckTimer* at);
    /*
     * Send message with ack control
     */
    void sendMessageWithAck(MarketMessage* msg, const std::vector<int>& targets);
    /*
     * Check if this vehicle is the intended target of a message
     */
    bool isReceiver(MarketMessage* msg);
    /*
     * Seconds to wait for ack messages
     */
    const double ackTime = 0.2;
private:
    /*
     * Manage the messages that are sent
     */
    MessageCache messageCache;
    /*
     * Timers
     */
    cMessage* debugTimer = NULL;
    void testFunction();
};

#endif /* APP_PLEXEAPPL_CONCRETE_MARKET_MARKETAGENT_H_ */
