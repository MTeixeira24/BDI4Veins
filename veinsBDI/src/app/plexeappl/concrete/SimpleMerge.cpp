/*
 * SimpleMerge.cpp
 *
 *  Created on: 28/08/2018
 *      Author: miguel
 */

#include "../concrete/SimpleMerge.h"

Define_Module(SimpleMerge)

SimpleMerge::SimpleMerge() {

}

SimpleMerge::~SimpleMerge() {
}

void SimpleMerge::initialize(int stage)
{
    BasePlexeAgentAppl::initialize(stage);
    if (stage == 1){
        // connect maneuver application to protocol
        protocol->registerApplication(MANEUVER_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"), gate("lowerControlIn"), gate("lowerControlOut"));
        //In order to simplify the scenario, we know that agent with ID 4 is the arterial vehicle so we send that belief to the agent
        if(myId == 4){
            BeliefModel bm;
            bm.setBelief("merge");
            int value = 1;
            bm.pushInt(&value);
            manager->sendInformationToAgents(myId, &bm);
        }
    }

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

void SimpleMerge::startMerge(int platoonId, int destinationId, int position)
{
    /*ASSERT(getPlatoonRole() == PlatoonRole::NONE);
    ASSERT(!isInManeuver());

    msg->setKind(MANEUVER_TYPE);
    msg->setVehicleId(vehicleId);
    msg->setExternalId(externalId.c_str());
    msg->setPlatoonId(platoonId);
    msg->setDestinationId(destinationId);

    JoinManeuverParameters params;
    params.platoonId = platoonId;
    params.leaderId = leaderId;
    params.position = position;
    joinManeuver->startManeuver(&params);

     // collect information about target Platoon
    targetPlatoonData.reset(new TargetPlatoonData());
    targetPlatoonData->platoonId = pars->platoonId;
    targetPlatoonData->platoonLeader = pars->leaderId;

    // send join request to leader
    JoinPlatoonRequest* req = createJoinPlatoonRequest(positionHelper->getId(), positionHelper->getExternalId(), targetPlatoonData->platoonId, targetPlatoonData->platoonLeader, traciVehicle->getLaneIndex(), mobility->getCurrentPosition().x, mobility->getCurrentPosition().y);
    app->sendUnicast(req, targetPlatoonData->platoonLeader);

    */
    SimpleMergeMessage* msg = new SimpleMergeMessage("SimpleMergeMessage");
    msg->setKind(MANEUVER_TYPE);
    msg->setVehicleId(myId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
    msg->setPlatoonId(platoonId);
    msg->setDestinationId(destinationId);
    sendUnicast(msg, destinationId);
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
    UnicastMessage* unicast = check_and_cast<UnicastMessage*>(msg);

    cPacket* enc = unicast->getEncapsulatedPacket();
    ASSERT2(enc, "received a UnicastMessage with nothing inside");

    if (enc->getKind() == MANEUVER_TYPE) {
        ManeuverMessage* mm = check_and_cast<ManeuverMessage*>(unicast->decapsulate());
        if (SimpleMergeMessage* msg = dynamic_cast<SimpleMergeMessage*>(mm)) {
            //handleUpdatePlatoonFormation(msg);
            if (msg->getDestinationId() == myId){
                traciVehicle->setCACCConstantSpacing(15);
                // we have no data so far, so for the moment just initialize
                // with some fake data
                //traciVehicle->setLeaderVehicleFakeData(0, 0, 100);
                //traciVehicle->setFrontVehicleFakeData(0, 0, 100, 15);

                traciVehicle->setCruiseControlDesiredSpeed(25);
                traciVehicle->setActiveController(Plexe::ACC);
            }
            delete msg;
        }
        else {
            //onManeuverMessage(mm);
        }
        delete unicast;
    }
    else {
        BasePlexeAgentAppl::handleLowerMsg(msg);
    }
}

void SimpleMerge::onPlatoonBeacon(const PlatooningBeacon* pb)
{
    //joinManeuver->onPlatoonBeacon(pb);
    // maintain platoon
    BasePlexeAgentAppl::onPlatoonBeacon(pb);
}

