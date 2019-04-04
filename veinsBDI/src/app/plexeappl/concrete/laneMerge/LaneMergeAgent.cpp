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
    if(debugTimer != NULL)
        cancelAndDelete(debugTimer);
}

void LaneMergeAgent::initialize(int stage){
    GeneralPlexeAgentAppl::initialize(stage);
    if(stage == 2){
        messageCache.setSenderId(myId);
        protocol->registerApplication(NEGOTIATION_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"),
                        gate("lowerControlIn"), gate("lowerControlOut"));
        traciVehicle->setSpeed(mobility->getSpeed());
        traciVehicle->setFixedLane(traciVehicle->getLaneIndex(), false);
    }

}

void LaneMergeAgent::setInitialBeliefs(){
    //Setup beliefs
    int isMerger = myId % 2 == 0 ? true : false;
    Trigger beliefs(Belief("setup/beliefs"), myId, isMerger);
//    BeliefModel beliefs("setup/beliefs");
//    beliefs.pushInt(&isMerger);
    if(isMerger){
        startMergeTimer = new cMessage("startMergeTimer");
        scheduleAt(simTime()+1, startMergeTimer);
    }
    manager->QueueTrigger(beliefs);
    //manager->sendInformationToAgents(myId, &beliefs);
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
    sendMessageDelayed(msg, targetId);
}

void LaneMergeAgent::sendDecision(int targetId, short decision){
    Enter_Method_Silent();
    BargainMessage* msg = new BargainMessage("decision");
    fillNegotiationMessage(msg, myId, targetId);
    msg->setMessageType((int)MessageType::DECISION);
    msg->setData(decision);
    sendMessageDelayed(msg, targetId);
}

void LaneMergeAgent::sendPayout(int targetId, int payout){
    Enter_Method_Silent();
    BargainMessage* msg = new BargainMessage("payout");
    fillNegotiationMessage(msg, myId, targetId);
    msg->setMessageType((int)MessageType::PAYOUT);
    msg->setData(payout);
    sendMessageDelayed(msg, targetId);
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
    int targetId = msg->getVehicleId();
    int amount = msg->getData();

    switch(msg->getMessageType()){
    case (int)MessageType::OFFER:{
        if(!messageCache.hasReceived(msg->getMessageId())){
            messageCache.saveReceived(msg->getMessageId());
            msgName = "OfferAck";
            Trigger offerReceived(Belief("bargain/receive"), myId,targetId);
            offerReceived.appendInt(amount);
            manager->QueueTrigger(offerReceived);
//            BeliefModel offerReceived("bargain/receive");
//            offerReceived.pushInt(&targetId);
//            offerReceived.pushInt(&amount);
//            manager->sendInformationToAgents(myId, &offerReceived);
        }
        break;
    }
    case (int)MessageType::DECISION:{
        if(!messageCache.hasReceived(msg->getMessageId())){
            messageCache.saveReceived(msg->getMessageId());
            msgName = "DecisionAck";

            Trigger decisionReceived(Belief("bargain/receive/result"), myId,targetId);
            decisionReceived.appendInt(amount);
            manager->QueueTrigger(decisionReceived);
//            BeliefModel decisionReceived("bargain/receive/result");
//            decisionReceived.pushInt(&targetId);
//            decisionReceived.pushInt(&amount);
//            manager->sendInformationToAgents(myId, &decisionReceived);
        }
        break;
    }
    case (int)MessageType::PAYOUT:{
        if(!messageCache.hasReceived(msg->getMessageId())){
            messageCache.saveReceived(msg->getMessageId());
            msgName = "PayoutAck";

            Trigger payReceived(Belief("bargain/receive/payout"), myId,targetId);
            payReceived.appendInt(amount);
            manager->QueueTrigger(payReceived);
//            BeliefModel payReceived("bargain/receive/payout");
//            payReceived.pushInt(&targetId);
//            payReceived.pushInt(&amount);
//            manager->sendInformationToAgents(myId, &payReceived);
        }
        break;
    }
    case (int)MessageType::ACK:{
        messageCache.markReceived(msg->getReplyMessageId(), msg->getVehicleId());
        break;
    }
    case (int)MessageType::ACK_PAYOUT:{
        messageCache.markReceived(msg->getReplyMessageId(), msg->getVehicleId());
        if(traciVehicle->getLaneIndex() % 2 == 0)
            traciVehicle->setFixedLane(traciVehicle->getLaneIndex() + 1, false);
        break;
    }
    default:
        msgName = "0";
        break;
    }

    if(msg->getMessageType() < (int)MessageType::ACK){
        BargainMessage* reply = new BargainMessage(msgName);
        fillNegotiationMessage(reply, myId, msg->getVehicleId(), msg->getMessageId());
        if(msg->getMessageType() == (int)MessageType::PAYOUT)
            reply->setMessageType((int)MessageType::ACK_PAYOUT);
        else
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
    }else if(msg == startMergeTimer){
        delete msg;
        //traciVehicle->setFixedLane(traciVehicle->getLaneIndex() + 1, false);
        Trigger beliefs(Belief("bargain/start"), myId, myId + 1);
        manager->QueueTrigger(beliefs);
//        BeliefModel beliefs("bargain/start");
//        int targetVehicle = myId + 1;
//        beliefs.pushInt(&targetVehicle);
//        manager->sendInformationToAgents(myId, &beliefs);
    }else if(MarketMessage* delayMessage = dynamic_cast<MarketMessage*>(msg)){
        switch(delayMessage->getMessageType()){
        case (int)MessageType::ACK_PAYOUT:
        case (int)MessageType::ACK:
            sendUnicast(delayMessage, delayMessage->getDestinationId());
            break;
        default:
            sendMessageWithAck(delayMessage, delayMessage->getDestinationId());
            break;
        }
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
