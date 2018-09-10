/*
 * BaseJoin.cpp
 *
 *  Created on: 10/09/2018
 *      Author: miguel
 */

#include "BaseJoin.h"
#include "../app/plexeappl/GeneralPlexeAgentAppl.h"

BaseJoin::BaseJoin(GeneralPlexeAgentAppl* app)
    : BaseManeuver(app)
{
}

void BaseJoin::onManeuverMessage(const ManeuverMessage* mm)
{
    if (const JoinPlatoonRequest* msg = dynamic_cast<const JoinPlatoonRequest*>(mm)) {
        handleJoinPlatoonRequest(msg);
    }
    else if (const JoinPlatoonResponse* msg = dynamic_cast<const JoinPlatoonResponse*>(mm)) {
        handleJoinPlatoonResponse(msg);
    }
    else if (const MoveToPosition* msg = dynamic_cast<const MoveToPosition*>(mm)) {
        handleMoveToPosition(msg);
    }
    else if (const MoveToPositionAck* msg = dynamic_cast<const MoveToPositionAck*>(mm)) {
        handleMoveToPositionAck(msg);
    }
    else if (const JoinFormation* msg = dynamic_cast<const JoinFormation*>(mm)) {
        handleJoinFormation(msg);
    }
    else if (const JoinFormationAck* msg = dynamic_cast<const JoinFormationAck*>(mm)) {
        handleJoinFormationAck(msg);
    }
}

JoinPlatoonRequest* BaseJoin::createJoinPlatoonRequest(int vehicleId, std::string externalId, int platoonId, int destinationId, int currentLaneIndex, double xPos, double yPos)
{
    JoinPlatoonRequest* msg = new JoinPlatoonRequest("JoinPlatoonRequest");
    app->fillManeuverMessage(msg, vehicleId, externalId, platoonId, destinationId);
    msg->setCurrentLaneIndex(currentLaneIndex);
    msg->setXPos(xPos);
    msg->setYPos(yPos);
    return msg;
}

JoinPlatoonResponse* BaseJoin::createJoinPlatoonResponse(int vehicleId, std::string externalId, int platoonId, int destinationId, bool permitted)
{
    JoinPlatoonResponse* msg = new JoinPlatoonResponse("JoinPlatoonResponse");
    app->fillManeuverMessage(msg, vehicleId, externalId, platoonId, destinationId);
    msg->setPermitted(permitted);
    return msg;
}

MoveToPosition* BaseJoin::createMoveToPosition(int vehicleId, std::string externalId, int platoonId, int destinationId, double platoonSpeed, int platoonLane, const std::vector<int>& newPlatoonFormation)
{
    MoveToPosition* msg = new MoveToPosition("MoveToPosition");
    app->fillManeuverMessage(msg, vehicleId, externalId, platoonId, destinationId);
    msg->setPlatoonSpeed(platoonSpeed);
    msg->setPlatoonLane(platoonLane);
    msg->setNewPlatoonFormationArraySize(newPlatoonFormation.size());
    for (unsigned int i = 0; i < newPlatoonFormation.size(); i++) {
        msg->setNewPlatoonFormation(i, newPlatoonFormation[i]);
    }
    return msg;
}

MoveToPositionAck* BaseJoin::createMoveToPositionAck(int vehicleId, std::string externalId, int platoonId, int destinationId, double platoonSpeed, int platoonLane, const std::vector<int>& newPlatoonFormation)
{
    MoveToPositionAck* msg = new MoveToPositionAck("MoveToPositionAck");
    app->fillManeuverMessage(msg, vehicleId, externalId, platoonId, destinationId);
    msg->setPlatoonSpeed(platoonSpeed);
    msg->setPlatoonLane(platoonLane);
    msg->setNewPlatoonFormationArraySize(newPlatoonFormation.size());
    for (unsigned int i = 0; i < newPlatoonFormation.size(); i++) {
        msg->setNewPlatoonFormation(i, newPlatoonFormation[i]);
    }
    return msg;
}

JoinFormation* BaseJoin::createJoinFormation(int vehicleId, std::string externalId, int platoonId, int destinationId, double platoonSpeed, int platoonLane, const std::vector<int>& newPlatoonFormation)
{
    JoinFormation* msg = new JoinFormation("JoinFormation");
    app->fillManeuverMessage(msg, vehicleId, externalId, platoonId, destinationId);
    msg->setPlatoonSpeed(platoonSpeed);
    msg->setPlatoonLane(platoonLane);
    msg->setNewPlatoonFormationArraySize(newPlatoonFormation.size());
    for (unsigned int i = 0; i < newPlatoonFormation.size(); i++) {
        msg->setNewPlatoonFormation(i, newPlatoonFormation[i]);
    }
    return msg;
}

JoinFormationAck* BaseJoin::createJoinFormationAck(int vehicleId, std::string externalId, int platoonId, int destinationId, double platoonSpeed, int platoonLane, const std::vector<int>& newPlatoonFormation)
{
    JoinFormationAck* msg = new JoinFormationAck("JoinFormationAck");
    app->fillManeuverMessage(msg, vehicleId, externalId, platoonId, destinationId);
    msg->setPlatoonSpeed(platoonSpeed);
    msg->setPlatoonLane(platoonLane);
    msg->setNewPlatoonFormationArraySize(newPlatoonFormation.size());
    for (unsigned int i = 0; i < newPlatoonFormation.size(); i++) {
        msg->setNewPlatoonFormation(i, newPlatoonFormation[i]);
    }
    return msg;
}

