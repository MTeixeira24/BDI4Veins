/*
 * GeneralPlexeAgentAppl.cpp
 *
 *  Created on: 10/09/2018
 *      Author: miguel
 */

#include "GeneralPlexeAgentAppl.h"

Define_Module(GeneralPlexeAgentAppl);

void GeneralPlexeAgentAppl::initialize(int stage)
{
    BasePlexeAgentAppl::initialize(stage);
    if (stage == 1) {
        // connect maneuver application to protocol
        protocol->registerApplication(MANEUVER_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"), gate("lowerControlIn"), gate("lowerControlOut"));
        std::string joinManeuverName = par("joinManeuver").stdstringValue();
        if (joinManeuverName == "JoinAtN")
            joinManeuver = new JoinAtN(this);
        else if(joinManeuverName == "JoinAtBackAgent")
            joinManeuver = new JoinAtBackAgent(this);
        else
            throw new cRuntimeError("Invalid join maneuver implementation chosen");
        if(DynamicPositionManager::getInstance().vehToPlatoons.count(positionHelper->getId()) == 0){
            setPlatoonRole(PlatoonRole::NONE);
        }else if(positionHelper->isLeader()){
            setPlatoonRole(PlatoonRole::LEADER);
        }else{
            setPlatoonRole(PlatoonRole::FOLLOWER);
        }
    }
}

bool GeneralPlexeAgentAppl::isJoinAllowed() const
{
    return ((role == PlatoonRole::LEADER || role == PlatoonRole::NONE) && !inManeuver);
}

void GeneralPlexeAgentAppl::startJoinManeuver(int platoonId, int leaderId, int position)
{
    ASSERT(getPlatoonRole() == PlatoonRole::NONE);
    ASSERT(!isInManeuver());

    JoinManeuverParameters params;
    params.platoonId = platoonId;
    params.leaderId = leaderId;
    params.position = position;
    joinManeuver->startManeuver(&params);
}

void GeneralPlexeAgentAppl::sendUnicast(cPacket* msg, int destination)
{
    Enter_Method_Silent();
    take(msg);
    UnicastMessage* unicast = new UnicastMessage("UnicastMessage", msg->getKind());
    unicast->setDestination(destination);
    unicast->setChannel(Channels::CCH);
    unicast->encapsulate(msg);
    sendDown(unicast);
}

void GeneralPlexeAgentAppl::handleLowerMsg(cMessage* msg)
{
    UnicastMessage* unicast = check_and_cast<UnicastMessage*>(msg);

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
        BasePlexeAgentAppl::handleLowerMsg(msg);
    }
}

void GeneralPlexeAgentAppl::handleUpdatePlatoonFormation(const UpdatePlatoonFormation* msg)
{
    if (getPlatoonRole() != PlatoonRole::FOLLOWER) return;
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return;
    if (msg->getVehicleId() != positionHelper->getLeaderId()) return;

    // update formation information
    std::vector<int> f;
    for (unsigned int i = 0; i < msg->getPlatoonFormationArraySize(); i++) {
        f.push_back(msg->getPlatoonFormation(i));
    }
    positionHelper->setPlatoonFormation(f);
}

void GeneralPlexeAgentAppl::setPlatoonRole(PlatoonRole r)
{
    role = r;
}

void GeneralPlexeAgentAppl::onPlatoonBeacon(const PlatooningBeacon* pb)
{
    joinManeuver->onPlatoonBeacon(pb);
    // maintain platoon
    BasePlexeAgentAppl::onPlatoonBeacon(pb);
}

void GeneralPlexeAgentAppl::onManeuverMessage(ManeuverMessage* mm)
{
    joinManeuver->onManeuverMessage(mm);
    delete mm;
}

void GeneralPlexeAgentAppl::fillManeuverMessage(ManeuverMessage* msg, int vehicleId, std::string externalId, int platoonId, int destinationId)
{
    msg->setKind(MANEUVER_TYPE);
    msg->setVehicleId(vehicleId);
    msg->setExternalId(externalId.c_str());
    msg->setPlatoonId(platoonId);
    msg->setDestinationId(destinationId);
}

UpdatePlatoonFormation* GeneralPlexeAgentAppl::createUpdatePlatoonFormation(int vehicleId, std::string externalId, int platoonId, int destinationId, double platoonSpeed, int platoonLane, const std::vector<int>& platoonFormation)
{
    UpdatePlatoonFormation* msg = new UpdatePlatoonFormation("UpdatePlatoonFormation");
    fillManeuverMessage(msg, vehicleId, externalId, platoonId, destinationId);
    msg->setPlatoonSpeed(platoonSpeed);
    msg->setPlatoonLane(platoonLane);
    msg->setPlatoonFormationArraySize(platoonFormation.size());
    for (unsigned int i = 0; i < platoonFormation.size(); i++) {
        msg->setPlatoonFormation(i, platoonFormation[i]);
    }
    return msg;
}

void GeneralPlexeAgentAppl::finalizeManeuver(int joinerId){
}

GeneralPlexeAgentAppl::~GeneralPlexeAgentAppl()
{
    delete joinManeuver;
}

void GeneralPlexeAgentAppl::sendMessageWithAck(NegotiationMessage* msg, int target){
    std::vector<int> targets({target});
    sendMessageWithAck(msg, targets);
}

void GeneralPlexeAgentAppl::sendMessageWithAck(NegotiationMessage* msg, const std::vector<int>& targets){
    messageCache.insertEntry(msg->getMessageId(), msg->dup(), targets);
    AckTimer* at = new AckTimer("AckTimer");
    at->setMessageId(msg->getMessageId());
    scheduleAt(simTime() + ackTime + randomOffset(), at);
    sendUnicast(msg, -1);
}

double GeneralPlexeAgentAppl::randomOffset(){
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> distribution(25,5.0);
    return std::abs(distribution(gen) * 0.001);
}

bool GeneralPlexeAgentAppl::isReceiver(NegotiationMessage* msg){
    if(msg->getDestinationId() == myId)
        return true;
    if(msg->getTargets().find(myId) != msg->getTargets().end())
        return true;
    return false;
}

void GeneralPlexeAgentAppl::sendMessageDelayed(NegotiationMessage* msg, int target){
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> distribution(25,5.0);
    double delay = std::abs(distribution(gen) * 0.001);
    scheduleAt(simTime() + delay, msg);
}

void GeneralPlexeAgentAppl::resendMessage(long msgId, AckTimer* at){
    NegotiationMessage* resend = messageCache.getMessageReference(msgId);
    resend->setForWholePlatoon(false);
    resend->setTargets(messageCache.getRemainerIds(msgId));
    scheduleAt(simTime() + ackTime + randomOffset(), at);
    sendUnicast(resend->dup(), -1);
}
