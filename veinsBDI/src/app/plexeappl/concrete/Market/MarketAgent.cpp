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
    if(auctionTrigger != NULL)
        cancelAndDelete(auctionTrigger);
}

void MarketAgent::initialize(int stage){
    GeneralPlexeAgentAppl::initialize(stage);
    if (stage == 1){
        messageCache.setSenderId(myId);
        srand(time(NULL));
        protocol->registerApplication(NEGOTIATION_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"),
                gate("lowerControlIn"), gate("lowerControlOut"));
        //Setup beliefs
        BeliefModel beliefs("setup/beliefs");
        int isManager = false;
        int platoonId, leaderId, platoonSpeed;
        int wtp = ((MarketManager*)manager)->getWTP(myId), endowment = ((MarketManager*)manager)->getEndowment(myId);
        int preferredSpeed = ((MarketManager*)manager)->getPreferredSpeed(myId);
        std::vector<int> members, preferredPath = ((MarketManager*)manager)->getPreferredPath(myId);
        willingnessToPay = wtp;
        if(getPlatoonRole() == PlatoonRole::NONE){
            platoonId = -1;
            leaderId = -1;
            platoonSpeed = -1;
            members.push_back(myId);
            if(par("engageNegotiations").boolValue()){
                //What should the non platoon elements do to interact?
                joinerBehaviour();
            }
        }else{
            platoonId = positionHelper->getPlatoonId();
            leaderId = positionHelper->getLeaderId();
            platoonSpeed = (int)positionHelper->getPlatoonSpeed();
            members = positionHelper->getPlatoonFormation();
            //General platoon knowledge
            if (getPlatoonRole() == PlatoonRole::LEADER){
                isManager = true;
                //Leader specific knowledge
                leaderBehaviour();
            }else{
                //Member specific knowledge
                memberBehaviour();
            }
        }
        //Add to belief object
        beliefs.pushInt(&platoonId);
        beliefs.pushInt(&leaderId);
        beliefs.pushInt(&platoonSpeed);
        beliefs.pushInt(&isManager);
        beliefs.pushIntArray(members);
        beliefs.pushInt(&wtp);
        beliefs.pushInt(&endowment);
        beliefs.pushInt(&preferredSpeed);
        beliefs.pushIntArray(preferredPath);
        manager->sendInformationToAgents(myId, &beliefs);
    }
}


void MarketAgent::joinerBehaviour(){

}
void MarketAgent::leaderBehaviour(){
    auctionTrigger = new cMessage("auctionTriggerSpeed");
    scheduleAt(simTime() + 1, auctionTrigger);
    atc.context = CONTEXT_SPEED;
}
void MarketAgent::memberBehaviour(){

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
    Enter_Method_Silent();
    BidMessage* bid = new BidMessage("bid");
    fillNegotiationMessage(bid, myId, managerId, false);
    bid->setMessageType((int)MessageType::BID);
    bid->setAuctionId(auctionId);
    bid->setContext(context);
    bid->setBidValue(bidValue);
    bid->setWtp(willingnessToPay);
    sendMessageWithAck(bid, managerId);
}

void MarketAgent::sendAuctionResults(int auctionId, int auctionIteration, int winnerId){
    Enter_Method_Silent();
}


void MarketAgent::sendNotificationOfAuction(int auctionId, int context){
    Enter_Method_Silent();
    AuctionStatusMessage* notifyAuction = new AuctionStatusMessage("NotifyAuction");
    fillNegotiationMessage(notifyAuction, myId, -1, true);
    notifyAuction->setMessageType((int)MessageType::NOTIFY_AUCTION);
    notifyAuction->setAuctionId(auctionId);
    notifyAuction->setContext(context);
    notifyAuction->setManagerId(myId);
    sendMessageWithAck(notifyAuction, positionHelper->getPlatoonFormation());
}
void MarketAgent::handleAuctionStatusMessage(AuctionStatusMessage* msg){
    if(msg->getMessageType() == (int)MessageType::NOTIFY_AUCTION){
        if(!messageCache.hasReceived(msg->getMessageId())){
            std::cout << myId <<": Got notified of auction!" << std::endl;
            messageCache.saveReceived(msg->getMessageId());
            BeliefModel notify("receive/auction");
            int auctionId = msg->getAuctionId(), context = msg->getContext(), auctionManager = msg->getManagerId();
            notify.pushInt(&auctionId); notify.pushInt(&context); notify.pushInt(&auctionManager);
            manager->sendInformationToAgents(myId, &notify);
        }
        AuctionStatusMessage* reply = new AuctionStatusMessage("NotifyAuctionAck");
        fillNegotiationMessage(reply, myId, msg->getVehicleId(), false, msg->getMessageId());
        reply->setMessageType((int)MessageType::ACK);
        sendUnicast(reply, -1);
    }else if (msg->getMessageType() == (int)MessageType::ACK){
        messageCache.markReceived(msg->getReplyMessageId(), msg->getVehicleId());
    }
}

void MarketAgent::handleEndOfAuction(int auctionId, int auctionIteration, int winnerId){
    Enter_Method_Silent();
}
void MarketAgent::handleEndOfAuction(int auctionId, int auctionIteration, int winnerId, int pay, int wtpSum, int context){
    Enter_Method_Silent();
}

void MarketAgent::sendPay(int auctionId, int context, int pay, int managerId, int speed){
    Enter_Method_Silent();
}
void MarketAgent::sendPay(int auctionId, int context, int pay, int managerId, std::vector<int> route){
    Enter_Method_Silent();
}

void MarketAgent::distributePay(int auctionId, int auctionIteration, int winnerId, int payment, int wtpSum, int speed){
    Enter_Method_Silent();
}
void MarketAgent::distributePay(int auctionId, int auctionIteration, int winnerId, int payment, int wtpSum, std::vector<int> route){
    Enter_Method_Silent();
}


void MarketAgent::delegateNegotiationMessage(NegotiationMessage* nm){
    if(AuctionStatusMessage* msg = dynamic_cast<AuctionStatusMessage*>(nm)){
        if(isReceiver(msg)) handleAuctionStatusMessage(msg);
        delete msg;
    }/*else if(MarketMessage* msg = dynamic_cast<MarketMessage*>(nm)){
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
    }*/
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
    }else if(msg == auctionTrigger){
        BeliefModel startAuction("start/auction");
        startAuction.pushInt(&(atc.context));
        manager->sendInformationToAgents(myId, &startAuction);
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
