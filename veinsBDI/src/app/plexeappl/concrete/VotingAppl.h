/*
 * VotingAppl.h
 *
 *  Created on: 12/09/2018
 *      Author: miguel
 */

#ifndef APP_PLEXEAPPL_CONCRETE_VOTINGAPPL_H_
#define APP_PLEXEAPPL_CONCRETE_VOTINGAPPL_H_

#include "veins/modules/application/platooning/utilities/DynamicPositionHelper.h"

#include "../GeneralPlexeAgentAppl.h"
#include "../../../messages/NegotiationMessage_m.h"
#include "../../../messages/Ack_m.h"
#include "../../../messages/voting/RequestJoinPlatoonMessage_m.h"
#include "../../../messages/voting/SubmitVote_m.h"
#include "../../../messages/voting/NotifyResults_m.h"
#include "../../../messages/voting/NotifyVote_m.h"
#include "../../../messages/voting/RequestResults_m.h"

#include "../../../utilities/LeaderPositionHelper.h"
#include "../../../base/manager/constants/VoteConstants.h"
#include "../../../base/voteManager/VoteManager.h"

#include <random>

class VotingAppl : public GeneralPlexeAgentAppl {
public:
    VotingAppl() :
        searchingForPlatoon(false), negotiationState(VoteState::NONE){};
    virtual ~VotingAppl();

    /** override from GeneralPlexeAgentAppl */
    virtual void initialize(int stage) override;
    //TODO: Add this function to base agent
    virtual void sendToAgent(const BeliefModel* bm);
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
    };

    /**
    * Type identifers for Acks
    */
    enum class AckType : size_t {
       JOIN_REQUEST_RECEIVED,
       VOTE_RECEIVED
    };
    /**
     * Send a request to a platoon leader to join the platoon
     *
     * @param targetPlatoonId Id of the platoon leader
     */
    void sendRequestToJoin(int targetPlatoonId, int destinationId, double preferedSpeed, double tolerance);
    /**
     *
     */
    NotifyVote* fillNotificationOfVote(int contextId, std::vector<double>& contextArgs, std::vector<int>& candidates);
    void sendNotificationOfVoteGeneral(int contextId, std::vector<double>& contextArgs, std::vector<int>& candidates, int expectedVoteVector);
    void sendNotificationOfVoteDirect(VoteData electionData, int destinationId);
    /**
     *
     */
    void sendVoteSubmition(int vote);
    /**
     *
     */
    void sendVoteSubmition(std::vector<int>& votes);
    /**
     *
     */
    virtual void sendVoteResults(int winnerValue, int joinerId);
    /**
     * Send ack messages to confirm arrival of message
     */
    void sendAck(AckType, int);
    /**
     * Save the vote in the controller for future use
     */
    void setVote(std::vector<int>);
    /**
     * If  no results have been received. Request them directly
     */
    void sendResultRequest(int, int);
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
    virtual void handleRequestToJoinNegotiation(const RequestJoinPlatoonMessage* msg);

    /**
     *
     */
    void handleSubmitVote(const SubmitVote* msg);
    /**
     *
     */
    void handleNotificationOfResults(const NotifyResults* msg);
    /**
     *
     */
    void handleNotifyVote(const NotifyVote* msg);
    /**
     * Process received Ack messages
     */
    virtual void handleAck(const Ack* msg);
    /**
     * If a vehicle hasnt received the results, send it back
     */
    void handleRequestResults(RequestResults* msg);
    /**
     * Fill a negotiation message with the standard information
     */
    void fillNegotiationMessage(NegotiationMessage* msg, int originId, int targetId);
    /**
     *
     */
    virtual void delegateNegotiationMessage(NegotiationMessage* nm);
    enum class VoteState : size_t {
            NONE,
            AWAITING_ACK_SUBMIT,
            AWAITING_RESULTS,
            CHAIR_ELECTION_END,
            CHAIR_ELECTION_ONGOING,
            CHAIR_SEARCHING_JOINERS,
            JOINER_AWAITING_ACK_JOIN_REQUEST
     };
    /**
     *
     */
    void handleSelfMsg(cMessage* msg) override;
    /**
     * Save a copy of a message for resending if needed
     */
    void backupMessage(NegotiationMessage* msg);
    /*
     * Store data about the leader
     */
    int targetLeaderId = -1;
    /**
     * Set to true if agent has instructed the controller to search for open platoons
     */
    bool searchingForPlatoon;

    cMessage* searchTimer = NULL;

    cMessage* awaitAckTimer = NULL;
    /**
     * Self message of the leader to verify if all votes are received
     */
    cMessage* voteTimer = NULL;
    /**
     *
     */
    NegotiationMessage* copy = NULL;
    /**
     * Store this vehicles votes for resubmitting
     */
    std::vector<int> vote;

    /**
     * What state in the negotiation is this vehicle in right now?
     */
    VoteState negotiationState;
    /**
     * Used by the chair keep track of who submitted their votes
     */
    std::map<int, bool> received_votes;
    /**
     * Controller memory of the current election in order to resend failed messages
     */
    VoteData election_data;
};

#endif /* APP_PLEXEAPPL_CONCRETE_VOTINGAPPL_H_ */
