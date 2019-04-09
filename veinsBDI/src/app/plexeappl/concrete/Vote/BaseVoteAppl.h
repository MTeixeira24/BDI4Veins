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
#include "../../../../base/voteManager/VoteAgentManager.h"
class VoteAgentManager;
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
    enum class VoteState : size_t {
            NONE,
            AWAITING_ACK_SUBMIT,
            AWAITING_RESULTS,
            CHAIR_ELECTION_END,
            CHAIR_ELECTION_ONGOING,
            CHAIR_SEARCHING_JOINERS,
            JOINER_AWAITING_ACK_JOIN_REQUEST
     };
    void fillNegotiationMessage(NegotiationMessage* msg, int originId, int targetId,
            int idOfOriginMessage=-1, bool forWholePlatoon=false);
    void fillNegotiationMessage(NegotiationMessage* msg, int originId, int targetId,
            std::unordered_set<int>& targets, int idOfOriginMessage=-1,
            bool forWholePlatoon=true);

    virtual void handleLowerMsg(cMessage* msg) override;
    void handleSelfMsg(cMessage* msg) override;

    void delegateNegotiationMessage(NegotiationMessage* nm);
    void sendVoteSubmition(std::vector<int>& votes);
    void sendVoteResults(int winnerValue, int joinerId);
    void sendCommitteeVoteResults(std::vector<int>& results);
    void sendNotificationOfVoteGeneral(int contextId, std::vector<double>& contextArgs,
            std::vector<int>& candidates, int expectedVoteVector);

    void handleNotifyVote(const NotifyVote* msg);
    void handleSubmitVote(const SubmitVote* msg);
    void handleNotificationOfResults(const NotifyResults* msg);
protected:
    MessageCache messageCache;
    /*
     * Seconds to wait for ack messages
     */
    const double ackTime = 0.05;
    void sendMessageWithAck(NegotiationMessage* msg, int target);
    void sendMessageWithAck(NegotiationMessage* msg, const std::vector<int>& targets);
    double randomOffset();
    bool isReceiver(NegotiationMessage* msg);
    void sendMessageDelayed(NegotiationMessage* msg, int target);
    void resendMessage(long msgId, AckTimer* at);
    virtual void setInitialBeliefs() override;
    virtual void initialLeaderBehaviour();

    NotifyVote* fillNotificationOfVote(int contextId, std::vector<double>& contextArgs,
            std::vector<int>& candidates);
    std::map<int, bool> received_votes;
    cMessage* voteTimer;
    VoteAgentManager* voteManager;
    VoteData election_data;
    VoteState negotiationState;
    std::unordered_set<int> voteMembers;


private:
    std::vector<int> vote;
};

#endif /* APP_PLEXEAPPL_CONCRETE_VOTE_BASEVOTEAPPL_H_ */
