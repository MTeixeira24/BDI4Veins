/*
 * JoinAtN.cpp
 *
 *  Created on: 10/09/2018
 *      Author: miguel
 */

#include "JoinAtN.h"
#include "../app/plexeappl/GeneralPlexeAgentAppl.h"

JoinAtN::JoinAtN(GeneralPlexeAgentAppl* app)
    : BaseJoin(app)
    , joinManeuverState(JoinManeuverState::IDLE)
    , gapCreated(false)
    , joinerInPosition(false)
{
}

void JoinAtN::onManeuverMessage(const ManeuverMessage* mm)
{
    BaseJoin::onManeuverMessage(mm);
    if (const CreateGap* msg = dynamic_cast<const CreateGap*>(mm)) {
        handleCreateGapRequest(msg);
    }else if (const CreateGapAck* msg = dynamic_cast<const CreateGapAck*>(mm)) {
        handleCreateGapAck(msg);
    }else if (const CloseGap* msg = dynamic_cast<const CloseGap*>(mm)) {
        handleCloseGap(msg);
    }
}

void JoinAtN::startManeuver(const void* parameters)
{
    JoinManeuverParameters* pars = (JoinManeuverParameters*) parameters;
    if (joinManeuverState == JoinManeuverState::IDLE) {
        ASSERT(app->getPlatoonRole() == PlatoonRole::NONE);
        ASSERT(!app->isInManeuver());

        app->setInManeuver(true);
        app->setPlatoonRole(PlatoonRole::JOINER);

        // collect information about target Platoon
        targetPlatoonData.reset(new TargetPlatoonData());
        targetPlatoonData->platoonId = pars->platoonId;
        targetPlatoonData->platoonLeader = pars->leaderId;
        targetPlatoonData->joinIndex = pars->position;

        // send join request to leader
        JoinPlatoonRequest* req = createJoinPlatoonRequest(positionHelper->getId(), positionHelper->getExternalId(), targetPlatoonData->platoonId, targetPlatoonData->platoonLeader, traciVehicle->getLaneIndex(), mobility->getCurrentPosition().x, mobility->getCurrentPosition().y);
        app->sendUnicast(req, targetPlatoonData->platoonLeader);
        joinManeuverState = JoinManeuverState::J_WAIT_REPLY;
    }
}

void JoinAtN::abortManeuver()
{
}

void JoinAtN::onPlatoonBeacon(const PlatooningBeacon* pb)
{
    if (joinManeuverState == JoinManeuverState::J_MOVE_IN_POSITION) {
        // check correct role
        ASSERT(app->getPlatoonRole() == PlatoonRole::JOINER);

        // if the message comes from the leader
        if (pb->getVehicleId() == targetPlatoonData->newFormation.at(0)) {
            traciVehicle->setLeaderVehicleFakeData(pb->getControllerAcceleration(), pb->getAcceleration(), pb->getSpeed());
        }
        // if the message comes from the front vehicle
        int frontPosition = targetPlatoonData->joinIndex - 1;
        int frontId = targetPlatoonData->newFormation.at(frontPosition);
        if (pb->getVehicleId() == frontId) {
            // get front vehicle position
            Coord frontPosition(pb->getPositionX(), pb->getPositionY(), 0);
            // get my position
            Veins::TraCICoord traciPosition = mobility->getManager()->omnet2traci(mobility->getCurrentPosition());
            Coord position(traciPosition.x, traciPosition.y);
            // compute distance
            double distance = position.distance(frontPosition) - pb->getLength();
            traciVehicle->setFrontVehicleFakeData(pb->getControllerAcceleration(), pb->getAcceleration(), pb->getSpeed(), distance);
            traciVehicle->setCruiseControlDesiredSpeed(pb->getSpeed() + 10);
            // if we are in position, tell the leader about that
            if ((distance > 5) && (distance < 7)) { // TODO fixed value? make dependent on
                // controller and headway time
                // send move to position response to confirm the parameters
                MoveToPositionAck* ack = createMoveToPositionAck(positionHelper->getId(), positionHelper->getExternalId(), targetPlatoonData->platoonId, targetPlatoonData->platoonLeader, targetPlatoonData->platoonSpeed, targetPlatoonData->platoonLane, targetPlatoonData->newFormation);
                app->sendUnicast(ack, targetPlatoonData->newFormation.at(0));
                joinManeuverState = JoinManeuverState::J_WAIT_JOIN;
                //traciVehicle->setCruiseControlDesiredSpeed(pb->getSpeed());
            }
        }
    }else if (joinManeuverState == JoinManeuverState::B_CREATING_GAP){
        ASSERT(app->getPlatoonRole() == PlatoonRole::FOLLOWER);
        int frontId = positionHelper->getFrontId();
        Coord frontPosition(pb->getPositionX(), pb->getPositionY(), 0);
        if (pb->getVehicleId() == frontId){
            Veins::TraCICoord traciPosition = mobility->getManager()->omnet2traci(mobility->getCurrentPosition());
            Coord position(traciPosition.x, traciPosition.y);
            // compute distance
            double distance = position.distance(frontPosition) - pb->getLength();
            if(distance > 14){
                joinManeuverState = JoinManeuverState::B_HOLD_GAP;
                CreateGapAck* cgk = createCreateGapAck(positionHelper->getId(), positionHelper->getExternalId(),positionHelper->getPlatoonId(), positionHelper->getLeaderId());
                app->sendUnicast(cgk, positionHelper->getLeaderId());
            }
        }
    }
}

void JoinAtN::handleJoinPlatoonRequest(const JoinPlatoonRequest* msg)
{

    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return;

    if (app->getPlatoonRole() != PlatoonRole::LEADER && app->getPlatoonRole() != PlatoonRole::NONE) return;

    //TODO: At this stage voting will occur to determine if joining is allowed
    bool permission = app->isJoinAllowed();

    // send response to the joiner
    JoinPlatoonResponse* response = createJoinPlatoonResponse(positionHelper->getId(), positionHelper->getExternalId(), msg->getPlatoonId(), msg->getVehicleId(), permission);
    app->sendUnicast(response, msg->getVehicleId());

    if (!permission) return;

    app->setInManeuver(true);
    app->setPlatoonRole(PlatoonRole::LEADER);

    // disable lane changing during maneuver
    traciVehicle->setFixedLane(traciVehicle->getLaneIndex());
    positionHelper->setPlatoonLane(traciVehicle->getLaneIndex());

    // save some data. who is joining?
    joinerData.reset(new JoinerData());
    joinerData->from(msg);

    // this was only to grant the request
    // now send the data about the platoon to the joiner
    // add the joiner to the choosen position of the platoon

    //TODO: Middle position for testing purposes
    int position = (positionHelper->getPlatoonFormation().size())/2;
    gapCreatorId = positionHelper->getMemberId(position);
    //Use the dynamicPositionHelper utility to add the joiner vehicle at the determined position
    //((DynamicPositionHelper*)positionHelper)->addVehicleToPlatoon(joinerData->joinerId, position, msg->getPlatoonId());
    joinerData->newFormation = positionHelper->getPlatoonFormation();

    std::vector<int>::iterator it = joinerData->newFormation.begin();
    it += position;
    joinerData->newFormation.insert(it, joinerData->joinerId);

    //joinerData->newFormation.push_back(joinerData->joinerId);
    MoveToPosition* mtp = createMoveToPosition(positionHelper->getId(), positionHelper->getExternalId(),
                positionHelper->getPlatoonId(), joinerData->joinerId,
                positionHelper->getPlatoonSpeed(), positionHelper->getPlatoonLane(),
                joinerData->newFormation);
    /*MoveToPositionN* mtp = createMoveToPositionN(positionHelper->getId(), positionHelper->getExternalId(),
            positionHelper->getPlatoonId(), joinerData->joinerId,
            positionHelper->getPlatoonSpeed(), positionHelper->getPlatoonLane(),
            joinerData->newFormation, positionHelper->getMemberId(position + 1), positionHelper->getMemberId(position - 1));*/
    app->sendUnicast(mtp, joinerData->joinerId);

    //Notify vehicle at intended join position to form a gap from its lead vehicle
    CreateGap* cg = createCreateGap(positionHelper->getId(), positionHelper->getExternalId(),positionHelper->getPlatoonId(), positionHelper->getMemberId(position), joinerData->joinerId);
    app->sendUnicast(cg, positionHelper->getMemberId(position));

    joinManeuverState = JoinManeuverState::L_WAIT_JOINER_IN_POSITION;
}

CreateGap* JoinAtN::createCreateGap(int vehicleId, std::string externalId, int platoonId, int destinationId, int joinerId){
    CreateGap* msg = new CreateGap("CreateGap");
    app->fillManeuverMessage(msg, vehicleId, externalId, platoonId, destinationId);
    msg->setJoinerId(joinerId);
    return msg;
}

CreateGapAck* JoinAtN::createCreateGapAck(int vehicleId, std::string externalId, int platoonId, int destinationId){
    CreateGapAck* msg = new CreateGapAck("CreateGapAck");
    app->fillManeuverMessage(msg, vehicleId, externalId, platoonId, destinationId);
    return msg;
}

CloseGap* JoinAtN::createCloseGap(int vehicleId, std::string externalId, int platoonId, int destinationId){
    CloseGap* msg = new CloseGap("CloseGap");
    app->fillManeuverMessage(msg, vehicleId, externalId, platoonId, destinationId);
    return msg;
}

MoveToPositionN* JoinAtN::createMoveToPositionN(int vehicleId, std::string externalId, int platoonId, int destinationId, double platoonSpeed, int platoonLane, const std::vector<int>& newPlatoonFormation, int position, int frontId, int backId){
    MoveToPositionN* msg = (MoveToPositionN*)createMoveToPosition(vehicleId, externalId, platoonId, destinationId, platoonSpeed, platoonLane, newPlatoonFormation);
    msg->setName("MoveToPositionN");
    msg->setLeadVehicleId(frontId);
    msg->setRearVehicleId(backId);
    msg->setInsertionPosition(position);
    return msg;
}

void JoinAtN::handleJoinPlatoonResponse(const JoinPlatoonResponse* msg)
{
    if (app->getPlatoonRole() != PlatoonRole::JOINER) return;
    if (joinManeuverState != JoinManeuverState::J_WAIT_REPLY) return;
    if (msg->getPlatoonId() != targetPlatoonData->platoonId) return;
    if (msg->getVehicleId() != targetPlatoonData->platoonLeader) return;

    // evaluate permission
    if (msg->getPermitted()) {
        // wait for information about the join maneuver
        joinManeuverState = JoinManeuverState::J_WAIT_INFORMATION;
        // disable lane changing during maneuver
        traciVehicle->setFixedLane(traciVehicle->getLaneIndex());
    }
    else {
        // abort maneuver
        joinManeuverState = JoinManeuverState::IDLE;
        app->setPlatoonRole(PlatoonRole::NONE);
        app->setInManeuver(false);
    }
}

void JoinAtN::handleMoveToPosition(const MoveToPosition* msg)
{
    if (app->getPlatoonRole() != PlatoonRole::JOINER) return;
    if (joinManeuverState != JoinManeuverState::J_WAIT_INFORMATION) return;
    if (msg->getPlatoonId() != targetPlatoonData->platoonId) return;
    if (msg->getVehicleId() != targetPlatoonData->platoonLeader) return;

    // the leader told us to move in position, we can start
    // save some data about the platoon
    targetPlatoonData->from(msg);

    // check for correct lane. if not in correct lane, change it
    // if this already is the platoon lane, join at the back (or v.v.)
    // if this is not the plaoon lane, we have to move into longitudinal
    // position
    int currentLane = traciVehicle->getLaneIndex();
    //If joiner is not joining at the back, stay in adjacent lane in order to overtake the other members
    if(targetPlatoonData->joinIndex < (targetPlatoonData->newFormation.size() - 1)){
        traciVehicle->setFixedLane(targetPlatoonData->platoonLane + 1);
    }else if (currentLane != targetPlatoonData->platoonLane) { //Otherwise move to platoon lane
        traciVehicle->setFixedLane(targetPlatoonData->platoonLane);
    }

    // approaching the platoon

    // activate faked CACC. this way we can approach the front car
    // using data obtained through GPS
    traciVehicle->setCACCConstantSpacing(5);
    // we have no data so far, so for the moment just initialize
    // with some fake data
    traciVehicle->setLeaderVehicleFakeData(0, 0, targetPlatoonData->platoonSpeed);
    traciVehicle->setFrontVehicleFakeData(0, 0, targetPlatoonData->platoonSpeed, 15);
    // set a CC speed higher than the platoon speed to approach it
    traciVehicle->setCruiseControlDesiredSpeed(targetPlatoonData->platoonSpeed + 20);
    traciVehicle->setActiveController(Plexe::FAKED_CACC);

    joinManeuverState = JoinManeuverState::J_MOVE_IN_POSITION;
}

void JoinAtN::handleMoveToPositionAck(const MoveToPositionAck* msg)
{
    if (app->getPlatoonRole() != PlatoonRole::LEADER) return;
    if (joinManeuverState != JoinManeuverState::L_WAIT_JOINER_IN_POSITION) return;
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return;
    if (msg->getVehicleId() != joinerData->joinerId) return;

    for (unsigned i = 0; i < msg->getNewPlatoonFormationArraySize(); i++) ASSERT(msg->getNewPlatoonFormation(i) == joinerData->newFormation.at(i));

    // the joiner is now in position and is ready to join
    joinerInPosition = true;
    if(gapCreated && joinerInPosition){// tell the joiner to join the platoon
        JoinFormation* jf = createJoinFormation(positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId(), joinerData->joinerId, positionHelper->getPlatoonSpeed(), traciVehicle->getLaneIndex(), joinerData->newFormation);
        app->sendUnicast(jf, joinerData->joinerId);
        joinManeuverState = JoinManeuverState::L_WAIT_JOINER_TO_JOIN;
    }
}

void JoinAtN::handleJoinFormation(const JoinFormation* msg)
{
    if (app->getPlatoonRole() != PlatoonRole::JOINER) return;
    if (joinManeuverState != JoinManeuverState::J_WAIT_JOIN) return;
    if (msg->getPlatoonId() != targetPlatoonData->platoonId) return;
    if (msg->getVehicleId() != targetPlatoonData->platoonLeader) return;

    for (unsigned i = 0; i < msg->getNewPlatoonFormationArraySize(); i++) ASSERT(msg->getNewPlatoonFormation(i) == targetPlatoonData->newFormation[i]);

    traciVehicle->setFixedLane(targetPlatoonData->platoonLane);
    // we got confirmation from the leader
    // switch from faked CACC to real CACC
    traciVehicle->setActiveController(Plexe::CACC);
    // set spacing to 5 meters to get close to the platoon
    traciVehicle->setCACCConstantSpacing(5);
    //traciVehicle->setFixedLane(targetPlatoonData->platoonLane);

    // update platoon information
    positionHelper->setPlatoonId(msg->getPlatoonId());
    positionHelper->setPlatoonLane(targetPlatoonData->platoonLane);
    positionHelper->setPlatoonSpeed(targetPlatoonData->platoonSpeed);
    std::vector<int> formation;
    for (unsigned i = 0; i < msg->getNewPlatoonFormationArraySize(); i++) formation.push_back(msg->getNewPlatoonFormation(i));
    ((DynamicPositionHelper*)positionHelper)->addVehicleToPlatoon(msg->getDestinationId(), formation.size() - 1, msg->getPlatoonId());
    positionHelper->setPlatoonFormation(formation);
    std::vector<int> newFormation = positionHelper->getPlatoonFormation();
    /*for(unsigned int i = 0; i < msg->getNewPlatoonFormationArraySize(); i++ ){
        if(msg->getNewPlatoonFormation(i) == msg->getDestinationId()){
            position = i;
            break;
        }
    }*/
    //((DynamicPositionHelper*)positionHelper)->addVehicleToPlatoon(msg->getDestinationId(), position, msg->getPlatoonId());
    traciVehicle->setCruiseControlDesiredSpeed(targetPlatoonData->platoonSpeed);

    // tell the leader that we're now in the platoon
    JoinFormationAck* jfa = createJoinFormationAck(positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId(), targetPlatoonData->platoonLeader, positionHelper->getPlatoonSpeed(), traciVehicle->getLaneIndex(), formation);
    app->sendUnicast(jfa, positionHelper->getLeaderId());

    app->setPlatoonRole(PlatoonRole::FOLLOWER);
    joinManeuverState = JoinManeuverState::IDLE;

    app->setInManeuver(false);
}

// final state for leader
// request update of formation information
void JoinAtN::handleJoinFormationAck(const JoinFormationAck* msg)
{
    if (app->getPlatoonRole() != PlatoonRole::LEADER) return;
    if (joinManeuverState != JoinManeuverState::L_WAIT_JOINER_TO_JOIN) return;
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return;
    if (msg->getVehicleId() != joinerData->joinerId) return;
    for (unsigned i = 0; i < msg->getNewPlatoonFormationArraySize(); i++){
        ASSERT(msg->getNewPlatoonFormation(i) == joinerData->newFormation.at(i));
    }

    // the joiner has joined the platoon
    // add the joiner to the list of vehicles in the platoon
    positionHelper->setPlatoonFormation(joinerData->newFormation);

    // send to all vehicles in Platoon
    for (int i = 1; i < positionHelper->getPlatoonSize(); i++) {
        UpdatePlatoonFormation* dup = app->createUpdatePlatoonFormation(positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId(), -1, positionHelper->getPlatoonSpeed(), traciVehicle->getLaneIndex(), joinerData->newFormation);
        int dest = positionHelper->getMemberId(i);
        dup->setDestinationId(dest);
        app->sendUnicast(dup, dest);
    }
    ASSERT(positionHelper->getMemberId(positionHelper->getMemberPosition(joinerData->joinerId) + 1) == gapCreatorId);
    CloseGap* cgk = createCloseGap(positionHelper->getId(), positionHelper->getExternalId(),positionHelper->getPlatoonId(), gapCreatorId);
    app->sendUnicast(cgk, gapCreatorId);

    joinManeuverState = JoinManeuverState::IDLE;
    app->setInManeuver(false);
    gapCreated = false;
    joinerInPosition = false;
}

void JoinAtN::handleCreateGapRequest(const CreateGap* msg){
    if (app->getPlatoonRole() != PlatoonRole::FOLLOWER) return;
    if (joinManeuverState != JoinManeuverState::IDLE) return;
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return;
    if (msg->getVehicleId() != positionHelper->getLeaderId()) return;


    traciVehicle->setCACCConstantSpacing(15);
    joinManeuverState = JoinManeuverState::B_CREATING_GAP;
}

void JoinAtN::handleCreateGapAck(const CreateGapAck* msg){
    if (app->getPlatoonRole() != PlatoonRole::LEADER) return;
    if (joinManeuverState != JoinManeuverState::L_WAIT_JOINER_IN_POSITION) return;
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return;

    gapCreated = true;
    if(gapCreated && joinerInPosition){// tell the joiner to join the platoon
        JoinFormation* jf = createJoinFormation(positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId(), joinerData->joinerId, positionHelper->getPlatoonSpeed(), traciVehicle->getLaneIndex(), joinerData->newFormation);
        app->sendUnicast(jf, joinerData->joinerId);
        joinManeuverState = JoinManeuverState::L_WAIT_JOINER_TO_JOIN;
    }
}

void JoinAtN::handleCloseGap(const CloseGap* msg){
    if (app->getPlatoonRole() != PlatoonRole::FOLLOWER) return;
    if (joinManeuverState != JoinManeuverState::B_HOLD_GAP) return;
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return;
    if (msg->getVehicleId() != positionHelper->getLeaderId()) return;

    traciVehicle->setCACCConstantSpacing(5);
    joinManeuverState = JoinManeuverState::IDLE;
}

