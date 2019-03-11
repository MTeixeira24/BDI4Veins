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
//        if(isMerger){
//            debugTimer = new cMessage("debugTimer");
//            scheduleAt(simTime()+1, debugTimer);
//        }
        manager->sendInformationToAgents(myId, &beliefs);
        traciVehicle->setSpeed(mobility->getSpeed());
        traciVehicle->setFixedLane(traciVehicle->getLaneIndex(), false);
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
        if(isReceiver(msg)) handleBargainMessage(msg);
        delete msg;
    }
}

void LaneMergeAgent::handleBargainMessage(BargainMessage* msg){
    const char *msgName;

    switch(msg->getMessageType()){
    case (int)MessageType::OFFER:{
        if(!messageCache.hasReceived(msg->getMessageId())){
            msgName = "OfferAck";
        }
        break;
    }
    case (int)MessageType::DECISION:{
        if(!messageCache.hasReceived(msg->getMessageId())){
            msgName = "DecisionAck";
        }
        break;
    }
    case (int)MessageType::PAYOUT:{
        if(!messageCache.hasReceived(msg->getMessageId())){
            msgName = "PayoutAck";
        }
        break;
    }
    case (int)MessageType::ACK:{
        messageCache.markReceived(msg->getReplyMessageId(), msg->getVehicleId());
        break;
    }
    default:
        msgName = "0";
        break;
    }

    if(msg->getMessageType() != (int)MessageType::ACK){
        BargainMessage* reply = new BargainMessage(msgName);
        fillNegotiationMessage(reply, myId, msg->getVehicleId(), msg->getMessageId());
        reply->setMessageType((int)MessageType::ACK);
        sendMessageDelayed(reply, msg->getVehicleId());
    }
}

void LaneMergeAgent::handleSelfMsg(cMessage* msg){
    if (AckTimer* at = dynamic_cast<AckTimer*>(msg)){
    long msgId = at->getMessageId();
        if(messageCache.allResponded(msgId)){
            delete at;
        }else{
            resendMessage(msgId, at);
        }
    }else if(msg == debugTimer){
        traciVehicle->setFixedLane(traciVehicle->getLaneIndex() + 1, false);
    } else{
        GeneralPlexeAgentAppl::handleSelfMsg(msg);
    }
}

bool LaneMergeAgent::isReceiver(MarketMessage* msg){
    if(msg->getDestinationId() == myId)
        return true;
    if(msg->getTargets().find(myId) != msg->getTargets().end())
        return true;
    return false;
}

void LaneMergeAgent::sendMessageDelayed(MarketMessage* msg, int target){
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> distribution(25,5.0);
    double delay = std::abs(distribution(gen) * 0.001);
    scheduleAt(simTime() + delay, msg);
}

void LaneMergeAgent::resendMessage(long msgId, AckTimer* at){
    MarketMessage* resend = messageCache.getMessageReference(msgId);
    resend->setForWholePlatoon(false);
    resend->setTargets(messageCache.getRemainerIds(msgId));
    scheduleAt(simTime() + ackTime + randomOffset(), at);
    sendUnicast(resend->dup(), -1);
}
