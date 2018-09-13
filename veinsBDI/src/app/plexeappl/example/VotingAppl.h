/*
 * VotingAppl.h
 *
 *  Created on: 12/09/2018
 *      Author: miguel
 */

#ifndef APP_PLEXEAPPL_EXAMPLE_VOTINGAPPL_H_
#define APP_PLEXEAPPL_EXAMPLE_VOTINGAPPL_H_

#include "../GeneralPlexeAgentAppl.h"
#include "../../../messages/NegotiationMessage_m.h"
#include "../../../messages/RequestJoinPlatoonMessage_m.h"
#include "../../../messages/NotificationOfJoinVote_m.h"

class VotingAppl : public GeneralPlexeAgentAppl {
public:
    VotingAppl();
    virtual ~VotingAppl();

    /** override from GeneralPlexeAgentAppl */
    virtual void initialize(int stage) override;
    //TODO: Add this function to base agent
    virtual void sendToAgent(const BeliefModel* bm);
    /**
     * Send a request to a platoon leader to join the platoon
     *
     * @param targetPlatoonId Id of the platoon leader
     */
    void sendRequestToJoin(int targetPlatoonId, int destinationId, double preferedSpeed, double tolerance);
    /**
     * Send a notification to platoon members that an election to decide entry of a new vehicle is about to start
     *
     * @param preferedspeed The joiner vehicles prefered speed
     * @param tolerance The joiner vehicles tolerance to deviations from its prefered speed
     */
    void sendNotificationOfJoinVote(double preferedspeed, double tolerance);
protected:
    /**
     * Extend from GeneralPlexeAgentAppl to handle messages related to voting
     */
    virtual void handleLowerMsg(cMessage* msg) override;
    /**
     * Handles PlatoonBeacons, extended to handle beacons related to voting
     *
     * @param PlatooningBeacon pb to handle
     */
    virtual void onPlatoonBeacon(const PlatooningBeacon* pb) override;

    /**
     * Handles requests received from a potential joiner vehicle by directing it to the agent to prepare negotiations
     *
     * @param msg The message received from the potential joiner
     */
    void handleRequestToJoinNegotiation(const RequestJoinPlatoonMessage* msg);
    /**
     * Handles notifications that alerts agents that an election to decide the entry of a new vehicle is about to start
     *
     * @param msg The message received from the leader.
     */
    void handleNotificationOfJoinVote(const NotificationOfJoinVote* msg);

    enum class InitialState {
            NONE,
            JOINER,
            FOLLOWER,
            LEADER
     };
private:
    /**
     * Set to true if agent has instructed the controller to search for open platoons
     */
    bool searchingForPlatoon;

};

#endif /* APP_PLEXEAPPL_EXAMPLE_VOTINGAPPL_H_ */
