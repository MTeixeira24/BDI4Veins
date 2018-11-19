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
protected:
    virtual void leaderInitialBehaviour() override;
    void handleSelfMsg(cMessage* msg) override;
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
        SENDER
    };
    LeaderRole leaderRole;
    enum class RegroupMsgTypes: uint8_t {
        OK,
        ACK,
        REQUEST,
        VOTE_DATA,
        ELECTION_RESULT
    };
};

#endif /* APP_PLEXEAPPL_CONCRETE_REGROUPAPPL_H_ */
