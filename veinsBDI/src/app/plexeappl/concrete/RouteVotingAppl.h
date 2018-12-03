/*
 * RouteVotingAppl.h
 *
 *  Created on: 14/10/2018
 *      Author: miguel
 */

#ifndef APP_PLEXEAPPL_CONCRETE_ROUTEVOTINGAPPL_H_
#define APP_PLEXEAPPL_CONCRETE_ROUTEVOTINGAPPL_H_

#include "VotingAppl.h"

#include "../../../messages/routeVoting/JoinProposal_m.h"

class RouteVotingAppl : public VotingAppl {
public:
    RouteVotingAppl() : cycle(VoteCycle::NONE){};
    virtual ~RouteVotingAppl();
    /** override from the normal appl */
    virtual void initialize(int stage) override;
    virtual void finalizeManeuver(int joinerId) override;
    virtual void sendVoteResults(int winnerValue, int joinerId) override;
    virtual void sendCommitteeVoteResults(std::vector<int>& results) override;
    virtual void handleEndOfVote() override;
protected:
    /**
     * Defines what behavior the leader should start with
     */
    virtual void leaderInitialBehaviour();
    /**
     * Fill out and send a proposal message
     */
    virtual void sendJoinProposal();
    /*
     *
     */
    void handleSelfMsg(cMessage* msg) override;
    /*
     * Timer to send proposals to join
     */
    cMessage* sendProposal = NULL;
    /*
     * Timer to send speed updates to the agent
     */
    cMessage* updateCurrentSpeed;
    /*
     *
     */
    cMessage* startSpeedVoteDelay = NULL;

    /**
     * Self message to start the initial vote
     */
    cMessage* startInitialVote = NULL;
    /*
     * Override from voting appl
     */
    virtual void handleRequestToJoinNegotiation(const RequestJoinPlatoonMessage* msg) override;
    /**
     *
     */
    virtual void handleAck(const Ack* msg) override;
    /**
     *
     */
    virtual void handleNotificationOfResults(const NotifyResults* msg) override;
    /**
     *
     */
    virtual void delegateNegotiationMessage(NegotiationMessage* nm) override;
    /**
     * Store the current cycle of voting
     */
    enum class VoteCycle : size_t {
            NONE,
            ROUTE_VOTE,
            SPEED_VOTE
     };
    VoteCycle cycle;
};

#endif /* APP_PLEXEAPPL_CONCRETE_ROUTEVOTINGAPPL_H_ */
