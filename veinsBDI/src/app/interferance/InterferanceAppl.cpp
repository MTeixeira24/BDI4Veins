/*
 * InterferanceAppl.cpp
 *
 *  Created on: 18/10/2018
 *      Author: miguel
 */

#include "InterferanceAppl.h"
using namespace Veins;
Define_Module(InterferanceAppl);

void InterferanceAppl::initialize(int stage){
    BaseApplLayer::initialize(stage);

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
        positionHelper = FindModule<BasePositionHelper*>::findSubModule(getParentModule());
        protocol = FindModule<BaseProtocol*>::findSubModule(getParentModule());
        positionHelper->setId(100 + positionHelper->getId());
        myId = positionHelper->getId();

        // connect application to protocol
        protocol->registerApplication(InterferingBeaconing::INTERFEARING_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"), gate("lowerControlIn"), gate("lowerControlOut"));

        recordData = new cMessage("recordData");
        // init statistics collection. round to 0.1 seconds
        SimTime rounded = SimTime(floor(simTime().dbl() * 1000 + 100), SIMTIME_MS);
        scheduleAt(rounded, recordData);
    }
}
