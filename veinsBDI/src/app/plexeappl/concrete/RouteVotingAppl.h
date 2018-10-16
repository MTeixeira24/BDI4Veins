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
    RouteVotingAppl(){};
    virtual ~RouteVotingAppl(){}
    /** override from the normal appl */
    virtual void initialize(int stage) override;
protected:
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
    cMessage* sendProposal;
    /*
     * Timer to send speed updates to the agent
     */
    cMessage* updateCurrentSpeed;
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
    virtual void delegateNegotiationMessage(NegotiationMessage* nm) override;
};

#endif /* APP_PLEXEAPPL_CONCRETE_ROUTEVOTINGAPPL_H_ */
