/*
 * BaseManeuver.h
 *
 *  Created on: 10/09/2018
 *      Author: miguel
 */

#ifndef MANEUVERS_BASEMANEUVER_H_
#define MANEUVERS_BASEMANEUVER_H_

class GeneralPlexeAgentAppl;

#include "veins/modules/application/platooning/utilities/BasePositionHelper.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"

#include "veins/modules/application/platooning/messages/ManeuverMessage_m.h"
#include "veins/modules/application/platooning/messages/PlatooningBeacon_m.h"
#include "veins/modules/application/platooning/messages/UpdatePlatoonFormation_m.h"



class BaseManeuver {
public:
    /**
     * Constructor
     *
     * @param app pointer to the generic application used to fetch parameters and inform it about a concluded maneuver
     */
    BaseManeuver(GeneralPlexeAgentAppl* app);
    virtual ~BaseManeuver(){};

    /**
     * This method is invoked by the generic application to start the maneuver
     *
     * @param parameters parameters passed to the maneuver
     */
    virtual void startManeuver(const void* parameters) = 0;
    /**
     * This method is invoked by the generic application to abort the maneuver
     */
    virtual void abortManeuver() = 0;

    /**
     * This method is invoked by the generic application when a maneuver message is received.
     * The maneuver must not free the memory of the message, as this might be needed by other maneuvers as well.
     */
    virtual void onManeuverMessage(const ManeuverMessage* mm) = 0;

    /**
     * This method is invoked by the generic application when a beacon message is received
     * The maneuver must not free the memory of the message, as this might be needed by other maneuvers as well.
     */
    virtual void onPlatoonBeacon(const PlatooningBeacon* pb) = 0;

protected:
    GeneralPlexeAgentAppl* app;
    BasePositionHelper* positionHelper;
    Veins::TraCIMobility* mobility;
    Veins::TraCICommandInterface* traci;
    Veins::TraCICommandInterface::Vehicle* traciVehicle;
};

#endif /* MANEUVERS_BASEMANEUVER_H_ */
