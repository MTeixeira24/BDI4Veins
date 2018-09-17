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

#include "BasePlexeAgentAppl.h"

#include "veins/modules/messages/WaveShortMessage_m.h"
#include "veins/base/messages/MacPkt_m.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"


Define_Module(BasePlexeAgentAppl);

void BasePlexeAgentAppl::initialize(int stage){
    BaseAgentAppl::initialize(stage);
    if (stage == 0) {
        // set names for output vectors
        // distance from front vehicle
        distanceOut.setName("distance");
        // relative speed w.r.t. front vehicle
        relSpeedOut.setName("relativeSpeed");
        // vehicle id
        nodeIdOut.setName("nodeId");
        // current speed
        speedOut.setName("speed");
        // vehicle position
        posxOut.setName("posx");
        posyOut.setName("posy");
        // vehicle acceleration
        accelerationOut.setName("acceleration");
        controllerAccelerationOut.setName("controllerAcceleration");
    }

    if (stage == 1) {
        mobility = Veins::TraCIMobilityAccess().get(getParentModule());
        traci = mobility->getCommandInterface();
        traciVehicle = mobility->getVehicleCommandInterface();
        positionHelper = Veins::FindModule<BasePositionHelper*>::findSubModule(getParentModule());
        protocol = Veins::FindModule<BaseProtocol*>::findSubModule(getParentModule());
        myId = positionHelper->getId(); //This causes the id to be repeated if various vtypes are used

        // connect application to protocol
        protocol->registerApplication(BaseProtocol::BEACON_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"), gate("lowerControlIn"), gate("lowerControlOut"));

        recordData = new cMessage("recordData");
        // init statistics collection. round to 0.1 seconds
        SimTime rounded = SimTime(floor(simTime().dbl() * 1000 + 100), SIMTIME_MS);
        scheduleAt(rounded, recordData);

        //Save pointer to LightJason Manager
        manager = Veins::FindModule<LightJasonManager*>::findSubModule(getParentModule()->getParentModule()); //The network is 2 modules up
        if(manager == nullptr){
            throw new cRuntimeError("LightJason Application: No manager found");
        }
        /*
         * Vehicle type must be specified
         */
        std::string aslFile = par("asl_file").stdstringValue();
        manager->subscribeVehicle(this, myId, traciVehicle->getVType(), aslFile);
    }
}

BasePlexeAgentAppl::~BasePlexeAgentAppl() {
    cancelAndDelete(recordData);
    recordData = nullptr;
    cancelAndDelete(stopSimulation);
    stopSimulation = nullptr;
}

void BasePlexeAgentAppl::sendMessage(uint8_t message_type, const void* args){
    MessageParameters* mp = (MessageParameters*) args;
    if(message_type == MESSAGE_UNICAST){
        SimpleMergeMessage* msg = new SimpleMergeMessage("SimpleMergeMessage");
        msg->setDestinationId(mp->targetId);
        msg->setKind(5); //This is just for debugging
        sendUnicast(msg, mp->targetId);
    }
}

void BasePlexeAgentAppl::handleLowerMsg(cMessage* msg)
{
    UnicastMessage* unicast = check_and_cast<UnicastMessage*>(msg);

    cPacket* enc = unicast->decapsulate();
    ASSERT2(enc, "received a UnicastMessage with nothing inside");

    if (enc->getKind() == BaseProtocol::BEACON_TYPE) {
        onPlatoonBeacon(check_and_cast<PlatooningBeacon*>(enc));
    }
    else {
        error("received unknown message type");
    }

    delete unicast;
}

void BasePlexeAgentAppl::logVehicleData(bool crashed)
{
    // get distance and relative speed w.r.t. front vehicle
    double distance, relSpeed;
    Plexe::VEHICLE_DATA data;
    traciVehicle->getRadarMeasurements(distance, relSpeed);
    traciVehicle->getVehicleData(&data);
    if (crashed) {
        distance = 0;
        stopSimulation = new cMessage("stopSimulation");
        scheduleAt(simTime() + SimTime(1, SIMTIME_MS), stopSimulation);
    }
    // write data to output files
    distanceOut.record(distance);
    relSpeedOut.record(relSpeed);
    nodeIdOut.record(myId);
    accelerationOut.record(data.acceleration);
    controllerAccelerationOut.record(data.u);
    speedOut.record(data.speed);
    posxOut.record(data.positionX);
    posyOut.record(data.positionY);
}

void BasePlexeAgentAppl::handleLowerControl(cMessage* msg)
{
    delete msg;
}

void BasePlexeAgentAppl::sendUnicast(cPacket* msg, int destination)
{
    Enter_Method_Silent();
    take(msg);

    UnicastMessage* unicast = new UnicastMessage("UnicastMessage", msg->getKind());
    unicast->setDestination(destination);

    unicast->setChannel(Channels::CCH);

    unicast->encapsulate(msg);
    sendDown(unicast);
}

void BasePlexeAgentAppl::handleSelfMsg(cMessage* msg)
{
    if (msg == recordData) {
        // log mobility data
        logVehicleData(traciVehicle->isCrashed());
        // re-schedule next event
        scheduleAt(simTime() + SimTime(100, SIMTIME_MS), recordData);
    }
    if (msg == stopSimulation) {
        endSimulation();
    }
}

void BasePlexeAgentAppl::onPlatoonBeacon(const PlatooningBeacon* pb)
{
    if (positionHelper->isInSamePlatoon(pb->getVehicleId())) {
        // if the message comes from the leader
        if (pb->getVehicleId() == positionHelper->getLeaderId()) {
            traciVehicle->setLeaderVehicleData(pb->getControllerAcceleration(), pb->getAcceleration(), pb->getSpeed(), pb->getPositionX(), pb->getPositionY(), pb->getTime());
        }
        // if the message comes from the vehicle in front
        if (pb->getVehicleId() == positionHelper->getFrontId()) {
            traciVehicle->setFrontVehicleData(pb->getControllerAcceleration(), pb->getAcceleration(), pb->getSpeed(), pb->getPositionX(), pb->getPositionY(), pb->getTime());
        }
        // send data about every vehicle to the CACC. this is needed by the consensus controller
        struct Plexe::VEHICLE_DATA vehicleData;
        vehicleData.index = positionHelper->getMemberPosition(pb->getVehicleId());
        vehicleData.acceleration = pb->getAcceleration();
        vehicleData.length = pb->getLength();
        vehicleData.positionX = pb->getPositionX();
        vehicleData.positionY = pb->getPositionY();
        vehicleData.speed = pb->getSpeed();
        vehicleData.time = pb->getTime();
        vehicleData.u = pb->getControllerAcceleration();
        vehicleData.speedX = pb->getSpeedX();
        vehicleData.speedY = pb->getSpeedY();
        vehicleData.angle = pb->getAngle();
        // send information to CACC
        traciVehicle->setVehicleData(&vehicleData);
    }
    delete pb;
}

