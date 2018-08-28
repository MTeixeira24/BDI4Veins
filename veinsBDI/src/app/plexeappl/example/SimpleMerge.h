/*
 * SimpleMerge.h
 *
 *  Created on: 28/08/2018
 *      Author: miguel
 */

#ifndef APP_PLEXEAPPL_EXAMPLE_SIMPLEMERGE_H_
#define APP_PLEXEAPPL_EXAMPLE_SIMPLEMERGE_H_

#include "../BasePlexeAgentAppl.h"

class SimpleMerge: public BasePlexeAgentAppl {
public:
    SimpleMerge();
    virtual ~SimpleMerge();

    /** override from BaseApp */
    virtual void initialize(int stage) override;

    /**
     * Request start of JoinManeuver to leader
     * @param int platoonId the id of the platoon to join
     * @param int leaderId the id of the leader of the platoon to join
     * @param int position the position where the vehicle should join.
     * Depending on the implementation, this parameter might be ignored
     */
    void startMerge(int platoonId, int leaderId, int position);

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
//    virtual void onManeuverMessage(ManeuverMessage* mm);

private:
    /** join maneuver implementation */
//    JoinManeuver* joinManeuver;
};

#endif /* APP_PLEXEAPPL_EXAMPLE_SIMPLEMERGE_H_ */
