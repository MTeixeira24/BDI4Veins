/*
 * MarketAgent.cpp
 *
 *  Created on: Dec 18, 2018
 *      Author: miguel
 */

#include "MarketAgent.h"
Define_Module(MarketAgent)

MarketAgent::~MarketAgent() {
    //Delete messages
}

void MarketAgent::initialize(int stage){
    GeneralPlexeAgentAppl::initialize(stage);
    if (stage == 1){
        protocol->registerApplication(NEGOTIATION_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"),
                gate("lowerControlIn"), gate("lowerControlOut"));
        if(getPlatoonRole() == PlatoonRole::NONE){
            if(par("engageNegotiations").boolValue()){
                //What should the non platoon elements do to interact?
            }
        }else{
            //General platoon knowledge
            if (getPlatoonRole() == PlatoonRole::LEADER){
                //Leader specific knowledge
            }
        }
    }
}

void MarketAgent::fillNegotiationMessage(MarketMessage* msg, int originId, int targetId, bool forWholePlatoon){
    msg->setDestinationId(targetId);
    msg->setKind(NEGOTIATION_TYPE);
    msg->setVehicleId(originId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
    msg->setForWholePlatoon(forWholePlatoon);//msg->setTargets(targets);
}

void MarketAgent::fillNegotiationMessage(MarketMessage* msg, int originId, int targetId,
        bool forWholePlatoon, std::unordered_set<int>& targets){
    fillNegotiationMessage(msg, originId, targetId, forWholePlatoon);
    msg->setForWholePlatoon(forWholePlatoon);//msg->setTargets(targets);
}

void MarketAgent::handleLowerMsg(cMessage* msg){

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

void MarketAgent::delegateNegotiationMessage(NegotiationMessage* nm){
    /*if (RequestJoinPlatoonMessage* msg = dynamic_cast<RequestJoinPlatoonMessage*>(nm)) {
        handleRequestToJoinNegotiation(msg);
        delete msg;
    }else if (SubmitVote* msg = dynamic_cast<SubmitVote*>(nm)) {
        handleSubmitVote(msg);
        delete msg;
    }else if (NotifyResults* msg = dynamic_cast<NotifyResults*>(nm)) {
        handleNotificationOfResults(msg);
        delete msg;
    }else if (NotifyVote* msg = dynamic_cast<NotifyVote*>(nm)) {
        if((msg->getDestinationId() == -1) || (msg->getDestinationId() == myId)){
            handleNotifyVote(msg);
        }
        delete msg;
    }else if(RequestResults* msg = dynamic_cast<RequestResults*>(nm)) {
        handleRequestResults(msg);
        delete msg;
    } else if (Ack* msg = dynamic_cast<Ack*>(nm)) {
        handleAck(msg);
        delete msg;
    }*/
}

void MarketAgent::handleSelfMsg(cMessage* msg){
    if (AckTimer* at = dynamic_cast<AckTimer*>(msg)){
        long msgId = at->getMessageId();
        if(messageCache.allResponded(msgId)){
            delete at;
        }else{
            resendMessage(msgId);
        }
    }else {
        GeneralPlexeAgentAppl::handleSelfMsg(msg);
    }
}

void MarketAgent::resendMessage(long msgId){
    MarketMessage* resend = messageCache.getMessageReference(msgId);
    resend->setForWholePlatoon(false);
    resend->setTargets(messageCache.getRemainerIds(msgId));
    sendUnicast(resend->dup(), -1);
}
