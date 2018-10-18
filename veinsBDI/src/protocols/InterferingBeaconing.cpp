/*
 * InterferingBeaconing.cpp
 *
 *  Created on: 18/10/2018
 *      Author: miguel
 */

#include "InterferingBeaconing.h"

Define_Module(InterferingBeaconing);

void InterferingBeaconing::sendPlatooningMessage(int destinationAddress){
    // vehicle's data to be included in the message
    Plexe::VEHICLE_DATA data;
    // get information about the vehicle via traci
    traciVehicle->getVehicleData(&data);

    // create and send beacon
    UnicastMessage* unicast = new UnicastMessage("", INTERFEARING_TYPE);
    unicast->setDestination(-1);
    unicast->setPriority(priority);
    unicast->setChannel(Channels::CCH);

    // create platooning beacon with data about the car
    PlatooningBeacon* pkt = new PlatooningBeacon();
    pkt->setControllerAcceleration(data.u);
    pkt->setAcceleration(data.acceleration);
    pkt->setSpeed(data.speed);
    pkt->setVehicleId(myId);
    pkt->setPositionX(data.positionX);
    pkt->setPositionY(data.positionY);
    // set the time to now
    pkt->setTime(data.time);
    pkt->setLength(length);
    pkt->setSpeedX(data.speedX);
    pkt->setSpeedY(data.speedY);
    pkt->setAngle(data.angle);
    // i generated the message, i send it
    pkt->setRelayerId(myId);
    pkt->setKind(BEACON_TYPE);
    pkt->setByteLength(packetSize);
    pkt->setSequenceNumber(seq_n++);

    // put platooning beacon into the message for the UnicastProtocol
    unicast->encapsulate(pkt);
    sendDown(unicast);
}
