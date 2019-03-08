/*
 * LaneMergeAgent.cpp
 *
 *  Created on: Feb 27, 2019
 *      Author: miguel
 */

#include "LaneMergeAgent.h"

Define_Module(LaneMergeAgent);

LaneMergeAgent::LaneMergeAgent() {
}

LaneMergeAgent::~LaneMergeAgent() {
}

void LaneMergeAgent::initialize(int stage){
    GeneralPlexeAgentAppl::initialize(stage);
    if(stage == 1){
        messageCache.setSenderId(myId);
        protocol->registerApplication(NEGOTIATION_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"),
                        gate("lowerControlIn"), gate("lowerControlOut"));
        //Setup beliefs
        BeliefModel beliefs("setup/beliefs");
        int isMerger = myId % 2 == 0 ? true : false;
        beliefs.pushInt(&isMerger);
        manager->sendInformationToAgents(myId, &beliefs);
    }

}

void LaneMergeAgent::fillNegotiationMessage(BargainMessage* msg, int originId, int targetId){
    long msgId = rand();
    msg->setMessageId(msgId);
    msg->setDestinationId(targetId);
    msg->setKind(NEGOTIATION_TYPE);
    msg->setVehicleId(originId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
}
void LaneMergeAgent::fillNegotiationMessage(BargainMessage* msg, int originId, int targetId,
       int idOfOriginMessage){
    msg->setReplyMessageId(idOfOriginMessage);
    fillNegotiationMessage(msg, originId, targetId);
}

void LaneMergeAgent::sendOffer(int targetId, int amount){
    Enter_Method_Silent();
    BargainMessage* msg = new BargainMessage("offer");
    fillNegotiationMessage(msg, myId, targetId);
    msg->setMessageType((int)MessageType::OFFER);
    msg->setData(amount);

}

void LaneMergeAgent::sendDecision(int targetId, short decision){
    Enter_Method_Silent();
    BargainMessage* msg = new BargainMessage("decision");
    fillNegotiationMessage(msg, myId, targetId);
    msg->setMessageType((int)MessageType::DECISION);
    msg->setData(decision);
}

void LaneMergeAgent::sendPayout(int targetId, int payout){
    Enter_Method_Silent();
    BargainMessage* msg = new BargainMessage("payout");
    fillNegotiationMessage(msg, myId, targetId);
    msg->setMessageType((int)MessageType::PAYOUT);
    msg->setData(payout);
}

void LaneMergeAgent::sendMessageWithAck(MarketMessage* msg, int target){
    std::vector<int> targets({target});
    messageCache.insertEntry(msg->getMessageId(), msg->dup(), targets);
    AckTimer* at = new AckTimer("AckTimer");
    at->setMessageId(msg->getMessageId());
    scheduleAt(simTime() + ackTime + randomOffset(), at);
    sendUnicast(msg, -1);
}

double LaneMergeAgent::randomOffset(){
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> distribution(25,5.0);
    return std::abs(distribution(gen) * 0.001);
}

void LaneMergeAgent::handleLowerMsg(cMessage* msg){

    UnicastMessage* unicast = check_and_cast<UnicastMessage*>(msg);

    cPacket* enc = unicast->getEncapsulatedPacket();
    ASSERT2(enc, "received a UnicastMessage with nothing inside");

    if (enc->getKind() == NEGOTIATION_TYPE) {
        NegotiationMessage* nm = check_and_cast<NegotiationMessage*>(unicast->decapsulate());
        delegateNegotiationMessage(nm);
        delete unicast;
    }
    else {
        GeneralPlexeAgentAppl::handleLowerMsg(msg);
    }
}

void LaneMergeAgent::delegateNegotiationMessage(NegotiationMessage* nm){
    if(BargainMessage* msg = dynamic_cast<BargainMessage*>(nm)){
        //if(isReceiver(msg)) handleBargainMessage(msg);
        delete msg;
    }
}

void LaneMergeAgent::handleBargainMessage(BargainMessage* msg){
    switch(msg->getMessageType()){
    case (int)MessageType::OFFER:{
        break;
    }
    case (int)MessageType::DECISION:{
        break;
    }
    case (int)MessageType::PAYOUT:{
        break;
    }
    default:
        break;
    }
    BargainMessage* reply = new BargainMessage("ack");
    fillNegotiationMessage(reply, myId, msg->getVehicleId(), msg->getMessageId());
    reply->setMessageType((int)MessageType::ACK);
    //sendMessageDelayed(reply, msg->getVehicleId());
}

void LaneMergeAgent::handleSelfMsg(cMessage* msg){
    GeneralPlexeAgentAppl::handleSelfMsg(msg);
}
