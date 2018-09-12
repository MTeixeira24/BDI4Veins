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

class VotingAppl : public GeneralPlexeAgentAppl {
public:
    VotingAppl();
    virtual ~VotingAppl();

    /** override from GeneralPlexeAgentAppl */
    virtual void initialize(int stage) override;
    //TODO: Add this function to base agent
    virtual void sendToAgent(const BeliefModel* bm);
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
     * Send a request to a platoon leader to join the platoon
     *
     * @param targetPlatoonId Id of the platoon leader
     */
    void sendRequestToJoin(int targetPlatoonId, int destinationId);

    /**
     * Handles requests received from a potential joiner vehicle by directing it to the agent to prepare negotiations
     *
     * @param msg The message received from the potential joiner
     */
    void handleRequestToJoinNegotiation(const NegotiationMessage* msg);

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
