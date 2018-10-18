/*
 * GeneralPlexeAgentAppl.h
 *
 *  Based on GeneralPlatooningApp
 *  Created on: 10/09/2018
 *      Author: miguel
 */

#ifndef APP_PLEXEAPPL_GENERALPLEXEAGENTAPPL_H_
#define APP_PLEXEAPPL_GENERALPLEXEAGENTAPPL_H_

#include <algorithm>
#include <memory>

#include "../../maneuvers/JoinAtBackAgent.h"
#include "veins/modules/application/platooning/messages/ManeuverMessage_m.h"
#include "veins/modules/mobility/traci/TraCIConstants.h"
#include "veins/modules/application/platooning/UnicastProtocol.h"
#include "veins/modules/application/platooning/protocols/BaseProtocol.h"
#include "veins/modules/mobility/traci/TraCIColor.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"

#include "BasePlexeAgentAppl.h"
#include "../../maneuvers/JoinAtN.h"
#include "veins/modules/application/platooning/utilities/DynamicPositionManager.h"

/** possible roles of this vehicle */
enum class PlatoonRole : size_t {
    NONE, ///< The vehicle is not in a Platoon
    LEADER, ///< The vehicle is the leader of its Platoon
    FOLLOWER, ///< The vehicle is a normal follower in its Platoon
    JOINER ///< The vehicle is in the process of joining a Platoon
};

/**
 * Based on Plexes's GeneralPlatooningApp.
 *
 * Provides the same functionality as Plexes own platooning app, but extends the
 * BaseAgentAppl for interaction with the connection manager
 */
class GeneralPlexeAgentAppl : public BasePlexeAgentAppl {
public:
    /** c'tor for GeneralPlatooningApp */
    GeneralPlexeAgentAppl()
        : inManeuver(false)
        , role(PlatoonRole::NONE)
        , joinManeuver(nullptr)
        {
        };

    /** d'tor for GeneralPlatooningApp */
    virtual ~GeneralPlexeAgentAppl();
public:
    /**
     * Returns the role of this car in the platoon
     *
     * @return PlatoonRole the role in the platoon
     * @see PlatoonRole
     */
    const PlatoonRole& getPlatoonRole() const
    {
        return role;
    }

    /**
     * Sets the role of this car in the platoon
     *
     * @param PlatoonRole r the role in the platoon
     * @see PlatoonRole
     */
    void setPlatoonRole(PlatoonRole r);

    /** override from BaseApp */
    virtual void initialize(int stage) override;

    /**
     * Request start of JoinManeuver to leader
     * @param int platoonId the id of the platoon to join
     * @param int leaderId the id of the leader of the platoon to join
     * @param int position the position where the vehicle should join.
     * Depending on the implementation, this parameter might be ignored
     */
    void startJoinManeuver(int platoonId, int leaderId, int position);

    /** Abort join maneuver */
    void abortJoinManeuver();

    /**
     * Returns whether this car is in a maneuver
     * @return bool true, if this car is in a maneuver, else false
     */
    bool isInManeuver() const
    {
        return inManeuver;
    }

    /**
     * Set whether this car is in a maneuver
     * @param bool b whether this car is in a maneuver
     */
    void setInManeuver(bool b = true)
    {
        inManeuver = b;
    }

    BasePositionHelper* getPositionHelper()
    {
        return positionHelper;
    }
    Veins::TraCIMobility* getMobility()
    {
        return mobility;
    }
    Veins::TraCICommandInterface* getTraci()
    {
        return traci;
    }
    Veins::TraCICommandInterface::Vehicle* getTraciVehicle()
    {
        return traciVehicle;
    }

    /**
     * Sends a unicast message
     *
     * @param cPacket msg message to be encapsulated into the unicast
     * message
     * @param int destination of the message
     */
    virtual void sendUnicast(cPacket* msg, int destination);

    /**
     * Fills members of a ManeuverMessage
     *
     * @param msg ManeuverMessage the message to be filled
     * @param int vehicleId the id of the sending vehicle
     * @param int platoonId the id of the platoon of the sending vehicle
     * @param int destinationId the id of the destination
     */
    void fillManeuverMessage(ManeuverMessage* msg, int vehicleId, std::string externalId, int platoonId, int destinationId);

    /**
     * Creates a UpdatePlatoonFormation message
     *
     * @param int vehicleId the id of the sending vehicle
     * @param int platoonId the id of the platoon of the sending vehicle
     * @param int destinationId the id of the destination
     * @param int platoonSpeed the speed of the platoon
     * @param int platoonLane the id of the lane of the platoon
     * @param std::vector<int> platoonFormation the new platoon formation
     */
    UpdatePlatoonFormation* createUpdatePlatoonFormation(int vehicleId, std::string externalId, int platoonId, int destinationId, double platoonSpeed, int platoonLane, const std::vector<int>& platoonFormation);

    /**
     * Handles a UpdatePlatoonFormation in the context of this
     * application
     *
     * @param UpdatePlatoonFormation msg to handle
     */
    virtual void handleUpdatePlatoonFormation(const UpdatePlatoonFormation* msg);
    /**
     * Get this vehicle id
     */
    virtual int getVehicleId(){return myId;};

    bool isJoinAllowed() const;

    /**
     *
     */
    virtual void finalizeManeuver(int joinerId);
protected:
    /** override this method of BaseApp. we want to handle it ourself */
    virtual void handleLowerMsg(cMessage* msg) override;

    /**
     * Handles PlatoonBeacons
     *
     * @param PlatooningBeacon pb to handle
     */
    virtual void onPlatoonBeacon(const PlatooningBeacon* pb) override;

    /**
     * Handles ManeuverMessages
     *
     * @param ManeuverMessage mm to handle
     */
    virtual void onManeuverMessage(ManeuverMessage* mm);

    /** am i in a maneuver? */
    bool inManeuver;

private:
    /** the role of this vehicle */
    PlatoonRole role;
    /** join maneuver implementation */
    BaseJoin* joinManeuver;
};

#endif /* APP_PLEXEAPPL_GENERALPLEXEAGENTAPPL_H_ */
