/*
 * SimpleMerge.cpp
 *
 *  Created on: 28/08/2018
 *      Author: miguel
 */

#include "SimpleMerge.h"

SimpleMerge::SimpleMerge() {
    // TODO Auto-generated constructor stub

}

SimpleMerge::~SimpleMerge() {
    // TODO Auto-generated destructor stub
}

void SimpleMerge::initialize(int stage)
{
    BasePlexeAgentAppl::initialize(stage);

   /* if (stage == 1) {
        // connect maneuver application to protocol
        protocol->registerApplication(MANEUVER_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"), gate("lowerControlIn"), gate("lowerControlOut"));

        std::string joinManeuverName = par("joinManeuver").stdstringValue();
        if (joinManeuverName == "JoinAtBack")
            joinManeuver = new JoinAtBack(this);
        else
            throw new cRuntimeError("Invalid join maneuver implementation chosen");
    }*/
}

void SimpleMerge::startMerge(int platoonId, int leaderId, int position)
{
    /*ASSERT(getPlatoonRole() == PlatoonRole::NONE);
    ASSERT(!isInManeuver());

    JoinManeuverParameters params;
    params.platoonId = platoonId;
    params.leaderId = leaderId;
    params.position = position;
    joinManeuver->startManeuver(&params);*/
}

void SimpleMerge::sendUnicast(cPacket* msg, int destination)
{
    Enter_Method_Silent();
    take(msg);
    UnicastMessage* unicast = new UnicastMessage("UnicastMessage", msg->getKind());
    unicast->setDestination(destination);
    unicast->setChannel(Channels::CCH);
    unicast->encapsulate(msg);
    sendDown(unicast);
}

void SimpleMerge::handleLowerMsg(cMessage* msg)
{
    /*UnicastMessage* unicast = check_and_cast<UnicastMessage*>(msg);

    cPacket* enc = unicast->getEncapsulatedPacket();
    ASSERT2(enc, "received a UnicastMessage with nothing inside");

    if (enc->getKind() == MANEUVER_TYPE) {
        ManeuverMessage* mm = check_and_cast<ManeuverMessage*>(unicast->decapsulate());
        if (UpdatePlatoonFormation* msg = dynamic_cast<UpdatePlatoonFormation*>(mm)) {
            handleUpdatePlatoonFormation(msg);
            delete msg;
        }
        else {
            onManeuverMessage(mm);
        }
        delete unicast;
    }
    else {
        BaseApp::handleLowerMsg(msg);
    }*/
}

void SimpleMerge::onPlatoonBeacon(const PlatooningBeacon* pb)
{
    //joinManeuver->onPlatoonBeacon(pb);
    // maintain platoon
    BasePlexeAgentAppl::onPlatoonBeacon(pb);
}

