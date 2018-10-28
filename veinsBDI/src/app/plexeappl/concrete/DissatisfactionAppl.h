/*
 * DissatisfactionAppl.h
 *
 *  Created on: 27/10/2018
 *      Author: miguel
 */

#ifndef APP_PLEXEAPPL_CONCRETE_DISSATISFACTIONAPPL_H_
#define APP_PLEXEAPPL_CONCRETE_DISSATISFACTIONAPPL_H_

#include "RouteVotingAppl.h"

class DissatisfactionAppl : public RouteVotingAppl {
public:
    DissatisfactionAppl(){};
    virtual ~DissatisfactionAppl();
    virtual void initialize(int stage) override;
    //Instruct any other joiners to enter the platoon. Otherwise start the votes
    virtual void finalizeManeuver(int joinerId) override;
protected:
    /**
     * Votes are made for three scenarios: Initial vote at formation, vote after joiners have entered and a vote
     * after some environmental conditions changes the allowed route/speed
     */
    enum class Stage : size_t {
            INITIAL,
            JOINERS,
            ENVIRONMENTAL
     };
    Stage stage;
    /*
     * Override from RouteVotingAppl. Aggregates all requests and create a list of joiners
     * skipping the vote to allow join
     */
    virtual void handleRequestToJoinNegotiation(const RequestJoinPlatoonMessage* msg) override;
    /*
     * Store all received requests
     */
    std::vector<int> potentialJoiners;

    /*
     * Implements actions to handle self-message to start the join
     * In this scenario skip the vote to join an instead sequentially instruct joiners to join the platoon.
     */
    void handleSelfMsg(cMessage* msg);
    /*
     * Self message to notify of when to handle the saved joiners
     */
    cMessage* callJoinersTimer;
    /**
     * Self message to start the initial vote
     */
    cMessage* startInitialVote;
    /**
     * Instead of starting with a join proposal, start by voting on speed and route
     * with the current elements
     */
     virtual void leaderInitialBehaviour() override;
     /**
     * Override in order to add the joiners after the initial voting
     */
    virtual void sendVoteResults(int winnerValue, int joinerId) override;
private:
    void callToJoin();
};

#endif /* APP_PLEXEAPPL_CONCRETE_DISSATISFACTIONAPPL_H_ */
