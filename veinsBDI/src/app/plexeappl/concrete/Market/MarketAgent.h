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
#include "../../../../base/MarketManager/MarketManager.h"
#include "../../../../messages/market/AuctionStatusMessage_m.h"
#include "../../../../messages/market/BidMessage_m.h"


class MarketAgent : public GeneralPlexeAgentAppl {
public:
    MarketAgent(){};
    virtual ~MarketAgent();

    /** override from GeneralPlexeAgentAppl */
    virtual void initialize(int stage) override;

    enum class MessageType{
        NONE,
        ACK,
        OK,
        HELLO,
        NOTIFY_AUCTION,
        BID,
        ITERATION_RESULTS,
        AUCTION_END,
        PAYMENT,
        DISTRIBUTION
    };

    struct AuctionTriggerContext{
        int context;
    };
protected:
    double randomOffset();
    /**
     *
     */
    virtual void delegateNegotiationMessage(NegotiationMessage* nm);
    /**
     * Extend from GeneralPlexeAgentAppl to handle messages related to market
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
    /*
     * Initial behaviours
     */
    virtual void joinerBehaviour();
    virtual void leaderBehaviour();
    virtual void memberBehaviour();
    /*
     * end of auction trigger
     */
    virtual void endOfAuctionTrigger(int winnerId);
    AuctionTriggerContext atc;
    cMessage* auctionTrigger = NULL;
    int willingnessToPay;
    std::vector<int> agentBidTuples;
    std::vector<int> wtpList;
    uint32_t auctionSize;
    std::vector<int> auctionMembers;
    /**
     * Message handlers
     */
    void handleAuctionStatusMessage(AuctionStatusMessage* msg);
    void handleBidMessage(BidMessage* msg);
public:
    /*
     * Methods to send messages
     */
    void sendBid(int auctionId, int context, int bidValue, int managerId);

    void sendAuctionResults(int auctionId, int auctionIteration, int winnerId);

    void sendNotificationOfAuction(int auctionId, int context);

    void handleEndOfAuction(int auctionId, int auctionIteration, int winnerId, int duePayment);
    void handleEndOfAuction(int auctionId, int auctionIteration, int winnerId, int pay, int wtpSum, int context);

    void sendPay(int auctionId, int context, int pay, int managerId, int speed);
    void sendPay(int auctionId, int context, int pay, int managerId, std::vector<int> route);

    virtual void distributePay(int auctionId, int auctionIteration, int winnerId, int payment, int wtpSum, int speed);
    void distributePay(int auctionId, int auctionIteration, int winnerId, int payment, int wtpSum, std::vector<int> route);
};

#endif /* APP_PLEXEAPPL_CONCRETE_MARKET_MARKETAGENT_H_ */
