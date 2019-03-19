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
#include <chrono>


#include "../../GeneralPlexeAgentAppl.h"
#include "../../../../messages/AckTimer_m.h"
#include "../../../../messages/utility/MessageCache.h"
#include "../../../../base/MergeManager/MergeManager.h"
#include "../../../../messages/bargain/bargain_m.h"
#include "../../../../messages/AckTimer_m.h"

class LaneMergeAgent : public GeneralPlexeAgentAppl {
public:
    LaneMergeAgent();
    virtual ~LaneMergeAgent();

    /** override from GeneralPlexeAgentAppl */
    virtual void initialize(int stage) override;
    void sendOffer(int targetId, int amount);
    void sendDecision(int targetId, short decision);
    void sendPayout(int targetId, int payout);

protected:

    void fillNegotiationMessage(BargainMessage* msg, int originId, int targetId);
    void fillNegotiationMessage(BargainMessage* msg, int originId, int targetId,
           int idOfOriginMessage);

    virtual void handleLowerMsg(cMessage* msg) override;
    void handleSelfMsg(cMessage* msg) override;

    void delegateNegotiationMessage(NegotiationMessage* nm);
    void handleBargainMessage(BargainMessage* msg);

    /*
     * Seconds to wait for ack messages
     */
    const double ackTime = 0.05;
    //Is this agent a merger?
    bool isMerger;

    void sendMessageWithAck(MarketMessage* msg, int target);

    double randomOffset();

    bool isReceiver(MarketMessage* msg);

    void sendMessageDelayed(MarketMessage* msg, int target);

    void resendMessage(long msgId, AckTimer* at);

    virtual void setInitialBeliefs() override;

private:
    enum class MessageType{
        NONE,
        OFFER,
        DECISION,
        PAYOUT,
        ACK,
        ACK_PAYOUT
    };
    /*
     * Manage the messages that are sent
     */
    MessageCache messageCache;
    /*
     * Timers
     */
    cMessage* debugTimer = NULL;
    cMessage* startMergeTimer = NULL;

    std::chrono::time_point<std::chrono::steady_clock> startTime1;
    std::chrono::time_point<std::chrono::steady_clock> startTimeMessageReceived;
    std::chrono::time_point<std::chrono::steady_clock> startTimeMessageToMessage;
    std::vector<double> deltaTimeStack;
    std::vector<double> deltaTimeStackMessageReceived;
    std::vector<double> deltaTimeStackMessageToMessage;
    bool startTimer = false;

};

#endif /* APP_PLEXEAPPL_CONCRETE_LANEMERGE_LANEMERGEAGENT_H_ */