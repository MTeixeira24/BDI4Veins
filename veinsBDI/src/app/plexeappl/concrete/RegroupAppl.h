/*
 * RegroupAppl.h
 *
 *  Created on: Nov 14, 2018
 *      Author: miguel
 */

#ifndef APP_PLEXEAPPL_CONCRETE_REGROUPAPPL_H_
#define APP_PLEXEAPPL_CONCRETE_REGROUPAPPL_H_

#include "RouteVotingAppl.h"
#include "../../../messages/regroupVoting/DataExchange_m.h"
#include "../../../messages/regroupVoting/MemberExchange_m.h"

class RegroupAppl : public RouteVotingAppl {
public:
    RegroupAppl() :
        regroupState(RegroupState::NONE)
    {};
    virtual ~RegroupAppl();
    virtual void handleEndOfVote() override;
    virtual void sendCommitteeVoteResults(std::vector<int>& results) override;
    virtual void sendVoteSubmition(std::vector<int>& votes) override;
protected:
    virtual void handleSubmitVote(const SubmitVote* msg) override;
    virtual void leaderInitialBehaviour() override;
    void handleSelfMsg(cMessage* msg) override;
    virtual void delegateNegotiationMessage(NegotiationMessage* msg) override;
    void createRegroupElection(MemberExchange* me);
    /**
     * Process data exchange messages
     */
    void handleDataExchange(DataExchange* de);
    /**
     * Process memberExchangeMessages
     */
    void handleMemberExchange(MemberExchange* me);
    /**
     * Store the current cycle of voting
     */
    virtual void fillContextVector(const NotifyVote* msg, std::vector<double>& contextArgs) override;
    enum class RegroupState: size_t {
            NONE,
            REQUESTING_EXCHANGE,
            AWAITING_ACK_ACCEPT,
            COMMITTEE_VOTE,
            COMMITTEE_VOTE_FINISHED,
            AWAITING_OTHER_RESULTS,
            AWAITING_WINNER_DETERMINATION
     };
    RegroupState regroupState;
    enum class LeaderRole: size_t {
        RECEIVER,
        SENDER,
        NONE
    };
    LeaderRole leaderRole = LeaderRole::NONE;
    enum class RegroupMsgTypes: uint8_t {
        OK,
        ACK,
        REQUEST,
        VOTE_DATA,
        ELECTION_RESULT
    };
    /*
     * Hold pointers to messages we are not ready to process
     */
    MemberExchange* buffer = NULL;
    DataExchange* databuffer = NULL;
    /**
     * hold the id of leader we dealing with
     */
    int targetLeaderId = -1;

    void sendDataExchange(RegroupMsgTypes msgType, int origin, int target, std::vector<int>& data);
public:
    void sendMemberExchange(RegroupMsgTypes type);
};

#endif /* APP_PLEXEAPPL_CONCRETE_REGROUPAPPL_H_ */
