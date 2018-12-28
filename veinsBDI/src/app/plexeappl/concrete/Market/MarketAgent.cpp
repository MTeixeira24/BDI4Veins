/*
 * MarketAgent.cpp
 *
 *  Created on: Dec 18, 2018
 *      Author: miguel
 */

#include "MarketAgent.h"
Define_Module(MarketAgent)

MarketAgent::~MarketAgent() {
    if(debugTimer != NULL)
        cancelAndDelete(debugTimer);
}

void MarketAgent::initialize(int stage){
    GeneralPlexeAgentAppl::initialize(stage);
    if (stage == 1){
        messageCache.setSenderId(myId);
        srand(time(NULL));
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
                debugTimer = new cMessage("debugTimerLeader");
                scheduleAt(simTime() + 1, debugTimer);
            }
        }
    }
}

void MarketAgent::fillNegotiationMessage(MarketMessage* msg, int originId, int targetId, bool forWholePlatoon){
    long msgId = rand();
    msg->setMessageId(msgId);
    msg->setDestinationId(targetId);
    msg->setKind(NEGOTIATION_TYPE);
    msg->setVehicleId(originId);
    msg->setExternalId(positionHelper->getExternalId().c_str());
    msg->setForWholePlatoon(forWholePlatoon);
}

void MarketAgent::fillNegotiationMessage(MarketMessage* msg, int originId, int targetId,
        bool forWholePlatoon, std::unordered_set<int>& targets){
    fillNegotiationMessage(msg, originId, targetId, forWholePlatoon);
    msg->setForWholePlatoon(forWholePlatoon);
    msg->setTargets(targets);
}

void MarketAgent::fillNegotiationMessage(MarketMessage* msg, int originId, int targetId,
        bool forWholePlatoon, int idOfOriginMessage){
    msg->setReplyMessageId(idOfOriginMessage);
    std::unordered_set<int> setTarget({targetId});
    fillNegotiationMessage(msg, originId, targetId, forWholePlatoon, setTarget);
}

bool MarketAgent::isReceiver(MarketMessage* msg){
    if(msg->getForWholePlatoon() && positionHelper->getPlatoonId() == msg->getPlatoonId())
        return true;
    if(msg->getTargets().find(myId) != msg->getTargets().end())
        return true;
    return false;
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

/*======================
 * Send message methods
 ======================*/

void MarketAgent::sendBid(int auctionId, int context, int bidValue, int managerId){

}

void MarketAgent::sendAuctionResults(int auctionId, int auctionIteration, int winnerId){

}


void MarketAgent::sendNotificationOfAuction(int auctionId, int context){

}


void MarketAgent::handleEndOfAuction(int auctionId, int auctionIteration, int winnerId){

}
void MarketAgent::handleEndOfAuction(int auctionId, int auctionIteration, int winnerId, int pay, int wtpSum, int context){

}

void MarketAgent::sendPay(int auctionId, int context, int pay, int managerId, int speed){

}
void MarketAgent::sendPay(int auctionId, int context, int pay, int managerId, std::vector<int> route){

}

void MarketAgent::distributePay(int auctionId, int auctionIteration, int winnerId, int payment, int wtpSum, int speed){

}
void MarketAgent::distributePay(int auctionId, int auctionIteration, int winnerId, int payment, int wtpSum, std::vector<int> route){

}


void MarketAgent::delegateNegotiationMessage(NegotiationMessage* nm){
    if(MarketMessage* msg = dynamic_cast<MarketMessage*>(nm)){
        if(isReceiver(msg)){
            if(myId != 0){
                if(msg->getMessageType() == (int)MessageType::HELLO){
                    if(!messageCache.hasReceived(msg->getMessageId())){
                        std::cout << "Got an hello!" << std::endl;
                        messageCache.saveReceived(msg->getMessageId());
                        MarketMessage* testMsg = new MarketMessage("Reply");
                        fillNegotiationMessage(testMsg, myId, msg->getVehicleId(), false, msg->getMessageId());
                        testMsg->setMessageType((int)MessageType::ACK);
                        sendMessageWithAck(testMsg, msg->getVehicleId());
                    }
                }else{
                    std::cout << "Got an ok!" << std::endl;
                    messageCache.markReceived(msg->getReplyMessageId(), msg->getVehicleId());
                }
            }else{
                std::cout << "Got a reply!" << std::endl;
                messageCache.markReceived(msg->getReplyMessageId(), msg->getVehicleId());
                MarketMessage* testMsg = new MarketMessage("Reply");
                fillNegotiationMessage(testMsg, myId, msg->getVehicleId(), false, msg->getMessageId());
                testMsg->setMessageType((int)MessageType::OK);
                sendUnicast(testMsg, -1);
            }
        }
        delete msg;
    }
}

void MarketAgent::handleSelfMsg(cMessage* msg){
    if (AckTimer* at = dynamic_cast<AckTimer*>(msg)){
        long msgId = at->getMessageId();
        if(messageCache.allResponded(msgId)){
            delete at;
        }else{
            resendMessage(msgId, at);
        }
    }else if(msg == debugTimer){
        testFunction();
    }else{
        GeneralPlexeAgentAppl::handleSelfMsg(msg);
    }
}

void MarketAgent::testFunction(){
    MarketMessage* testMsg = new MarketMessage("Test");
    fillNegotiationMessage(testMsg, myId, -1, true);
    testMsg->setMessageType((int)MessageType::HELLO);
    sendMessageWithAck(testMsg, positionHelper->getPlatoonFormation());
}

void MarketAgent::sendMessageWithAck(MarketMessage* msg, const std::vector<int>& targets){
    messageCache.insertEntry(msg->getMessageId(), msg->dup(), targets);
    AckTimer* at = new AckTimer("AckTimer");
    at->setMessageId(msg->getMessageId());
    scheduleAt(simTime() + ackTime, at);
    sendUnicast(msg, -1);
}

void MarketAgent::sendMessageWithAck(MarketMessage* msg, int target){
    std::vector<int> v_target({target});
    sendMessageWithAck(msg, v_target);
}

void MarketAgent::resendMessage(long msgId, AckTimer* at){
    MarketMessage* resend = messageCache.getMessageReference(msgId);
    resend->setForWholePlatoon(false);
    resend->setTargets(messageCache.getRemainerIds(msgId));
    scheduleAt(simTime() + ackTime, at);
    sendUnicast(resend->dup(), -1);
}
