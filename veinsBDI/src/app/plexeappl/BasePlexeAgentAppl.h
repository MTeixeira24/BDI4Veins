//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef APP_PLEXEAPPL_BASEPLEXEAGENTAPPL_H_
#define APP_PLEXEAPPL_BASEPLEXEAGENTAPPL_H_

#include "../baseappl/BaseAgentAppl.h"
#include "../../messages/SimpleMergeMessage_m.h"

#include "veins/base/modules/BaseApplLayer.h"
#include "veins/modules/application/platooning/UnicastProtocol.h"
#include "veins/modules/application/platooning/messages/PlatooningBeacon_m.h"
#include "veins/modules/application/platooning/CC_Const.h"
#include "veins/modules/application/platooning/utilities/BasePositionHelper.h"
#include "veins/modules/application/platooning/protocols/BaseProtocol.h"


class BaseProtocol;
/*
 * Reimplementation of plexe base class in order to function with LightJason
 */
class BasePlexeAgentAppl : public BaseAgentAppl {
public:
    BasePlexeAgentAppl(){
        recordData = 0;
        stopSimulation = nullptr;
    }
    virtual ~BasePlexeAgentAppl();
    virtual void initialize(int stage) override;
    /**
     * Sends a unicast message
     *
     * @param msg message to be encapsulated into the unicast message
     * @param destination id of the destination
     */
    virtual void sendUnicast(cPacket* msg, int destination);

protected:
    // id of this vehicle
    //int myId;

    //Veins::TraCIMobility* mobility;
    //Veins::TraCICommandInterface* traci;
    //Veins::TraCICommandInterface::Vehicle* traciVehicle;


    inline uint8_t Belief(std::string key){return manager->getBeliefId(key);}

    // determines position and role of each vehicle
    BasePositionHelper* positionHelper;

    // lower layer protocol
    BaseProtocol* protocol;

    //LightJason connection manager
    //LightJasonManager* manager;

    /**
     * Log data about vehicle
     */
    virtual void logVehicleData(bool crashed = false);

    // output vectors for mobility stats
    // id of the vehicle
    cOutVector nodeIdOut;
    // distance and relative speed
    cOutVector distanceOut, relSpeedOut;
    // speed and position
    cOutVector speedOut, posxOut, posyOut;
    // real acceleration and controller acceleration
    cOutVector accelerationOut, controllerAccelerationOut;

    // messages for scheduleAt
    cMessage* recordData;
    // message to stop the simulation in case of collision
    cMessage* stopSimulation;




protected:
    virtual void handleLowerMsg(cMessage* msg) override;
    virtual void handleSelfMsg(cMessage* msg) override;
    virtual void handleLowerControl(cMessage* msg) override;
    virtual void sendMessage(uint8_t, const void*) override;

    /**
     * Handles PlatoonBeacons
     */
    virtual void onPlatoonBeacon(const PlatooningBeacon* pb);
};

#endif /* APP_PLEXEAPPL_BASEPLEXEAGENTAPPL_H_ */
