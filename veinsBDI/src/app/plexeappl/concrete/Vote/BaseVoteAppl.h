/*
 * BaseVoteAppl.h
 *
 *  Created on: Apr 5, 2019
 *      Author: miguel
 */

#ifndef APP_PLEXEAPPL_CONCRETE_VOTE_BASEVOTEAPPL_H_
#define APP_PLEXEAPPL_CONCRETE_VOTE_BASEVOTEAPPL_H_

#include <random>
#include <unordered_set>
#include <chrono>

#include "../../GeneralPlexeAgentAppl.h"
#include "../../../../messages/AckTimer_m.h"
#include "../../../../messages/utility/MessageCache.h"
#include "../../../../messages/AckTimer_m.h"

#include "../../../../messages/routeVoting/JoinProposal_m.h"
#include "../../../../messages/NegotiationMessage_m.h"
#include "../../../../messages/voting/RequestJoinPlatoonMessage_m.h"
#include "../../../../messages/voting/SubmitVote_m.h"
#include "../../../../messages/voting/NotifyResults_m.h"
#include "../../../../messages/voting/NotifyVote_m.h"
#include "../../../../messages/voting/RequestResults_m.h"

#include "../../../../utilities/LeaderPositionHelper.h"
#include "../../../../base/manager/constants/VoteConstants.h"
#include "../../../../base/voteManager/VoteManager.h"

class BaseVoteAppl: public GeneralPlexeAgentAppl {
public:
    BaseVoteAppl();
    virtual ~BaseVoteAppl();
    virtual void initialize(int stage) override;
    /**
    * Data structure for the leader to hold data about the election without having to query the agent
    */
    struct VoteData{
       int contextId;
       int currentResult;
       int joinerId;
       int expectedVoteVectorSize;
       std::vector<double> contextArgs;
       std::vector<int> candidates;
       std::vector<int> committeeResult;
    };
    void fillNegotiationMessage(NegotiationMessage* msg, int originId, int targetId);
    void fillNegotiationMessage(NegotiationMessage* msg, int originId, int targetId,
           int idOfOriginMessage);

    virtual void handleLowerMsg(cMessage* msg) override;
    void handleSelfMsg(cMessage* msg) override;

    void delegateNegotiationMessage(NegotiationMessage* nm);
    void handleBargainMessage(BargainMessage* msg);
protected:
    MessageCache messageCache;
    /*
     * Seconds to wait for ack messages
     */
    const double ackTime = 0.05;
    void sendMessageWithAck(MarketMessage* msg, int target);
    double randomOffset();
    bool isReceiver(MarketMessage* msg);
    void sendMessageDelayed(MarketMessage* msg, int target);
    void resendMessage(long msgId, AckTimer* at);
    virtual void setInitialBeliefs() override;
};

#endif /* APP_PLEXEAPPL_CONCRETE_VOTE_BASEVOTEAPPL_H_ */
