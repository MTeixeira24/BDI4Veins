/*
 * MarketAgent.cpp
 *
 *  Created on: Dec 18, 2018
 *      Author: miguel
 */

#include "MarketAgent.h"
Define_Module(MarketAgent)

MarketAgent::~MarketAgent() {
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
        Trigger beliefs(Belief("setup/beliefs"), myId);
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
        beliefs.appendInt(platoonId);
        beliefs.appendInt(leaderId);
        beliefs.appendInt(platoonSpeed);
        beliefs.appendInt(isManager);
        beliefs.appendVector(members);
        beliefs.appendInt(wtp);
        beliefs.appendInt(endowment);
        beliefs.appendInt(preferredSpeed);
        beliefs.appendVector(preferredPath);
        manager->QueueTrigger(beliefs);
    }
}


void MarketAgent::joinerBehaviour(){

}
void MarketAgent::leaderBehaviour(){
    auctionTrigger = new cMessage("auctionTriggerSpeed");
    scheduleAt(simTime() + 1, auctionTrigger);
    atc.context = CONTEXT_SPEED;
    auctionSize = positionHelper->getPlatoonSize() - 1;
    agentBidTuples.reserve(auctionSize*2);
    wtpList.reserve(positionHelper->getPlatoonSize());
    std::vector<int> members = positionHelper->getPlatoonFormation();
    auctionMembers.insert(auctionMembers.begin(), members.begin(), members.end());
    for(uint32_t i = 0; i < members.size(); i++)
        wtpList.push_back(((MarketManager*)manager)->getWTP(members[i]));
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
    /*Must use the delayed sending when it is expected
    * for multiple nodes to send a message at the same
    * time. If it is not randomly delayed, the target
    * of the message will receive all at the same time
    * and may cause a cascade of dropped packets.
    * If this occurs the time taken to reach consensus
    * is greatly increased ([.1,.6]s -> [4,inf)s).
    */
    sendMessageDelayed(bid, managerId);
}
void MarketAgent::handleBidMessage(BidMessage* msg){
    if(msg->getMessageType() == (int)MessageType::BID){
        if(!messageCache.hasReceived(msg->getMessageId())){
            std::cout << myId <<": Got a bid from:" << msg->getVehicleId() <<  std::endl;
            messageCache.saveReceived(msg->getMessageId());
            agentBidTuples.push_back(msg->getVehicleId());
            agentBidTuples.push_back(msg->getBidValue());
            if(agentBidTuples.size() == auctionSize*2){
                Trigger bids(Belief("receive/bids"), myId, agentBidTuples);
                manager->QueueTrigger(bids);
                agentBidTuples.clear();
            }
        }
        BidMessage* reply = new BidMessage("BidAck");
        fillNegotiationMessage(reply, myId, msg->getVehicleId(), false, msg->getMessageId());
        reply->setMessageType((int)MessageType::ACK);
        sendMessageDelayed(reply, msg->getVehicleId());
    }else if (msg->getMessageType() == (int)MessageType::ACK){
        messageCache.markReceived(msg->getReplyMessageId(), msg->getVehicleId());
    }else if (msg->getMessageType() == (int)MessageType::PAYMENT){
        if(!messageCache.hasReceived(msg->getMessageId())){
            std::cout << myId <<": Got a payment from:" << msg->getVehicleId() <<  std::endl;
            int pay = msg->getBidValue(), speed, winnerWtp = msg->getWtp();
            Trigger payment(Belief(("distribute/pay")), myId, pay);
            std::vector<int> path;
            if(msg->getContext() == CONTEXT_PATH){
                path = msg->getPropertyList();
                payment.appendVector(path);
            }else{
                speed = msg->getProperty();
                payment.appendInt(speed);
            }
            payment.appendInt(winnerWtp);
            manager->QueueTrigger(payment);
        }
        BidMessage* reply = new BidMessage("PaymentAck");
        fillNegotiationMessage(reply, myId, msg->getVehicleId(), false, msg->getMessageId());
        reply->setMessageType((int)MessageType::ACK);
        sendMessageDelayed(reply, msg->getVehicleId());
    }
}

void MarketAgent::sendAuctionResults(int auctionId, int auctionIteration, int winnerId){
    Enter_Method_Silent();
    AuctionStatusMessage* auctionStatus = new AuctionStatusMessage("IterationResults");
    fillNegotiationMessage(auctionStatus, myId, -1, true);
    auctionStatus->setMessageType((int)MessageType::ITERATION_RESULTS);
    auctionStatus->setAuctionId(auctionId);
    auctionStatus->setAuctionIteration(auctionIteration);
    auctionStatus->setWinnerId(winnerId);
    sendMessageWithAck(auctionStatus, auctionMembers);
}


void MarketAgent::sendNotificationOfAuction(int auctionId, int context){
    Enter_Method_Silent();
    AuctionStatusMessage* notifyAuction = new AuctionStatusMessage("NotifyAuction");
    fillNegotiationMessage(notifyAuction, myId, -1, true);
    notifyAuction->setMessageType((int)MessageType::NOTIFY_AUCTION);
    notifyAuction->setAuctionId(auctionId);
    notifyAuction->setContext(context);
    if(context == CONTEXT_SPEED)
        ((MarketManager*)manager)->startTimeStampSpeed(simTime().dbl() * 1000);
    if(context == CONTEXT_PATH)
        ((MarketManager*)manager)->startTimeStampRoute(simTime().dbl() * 1000);
    notifyAuction->setManagerId(myId);
    sendMessageWithAck(notifyAuction, auctionMembers);
}
void MarketAgent::handleAuctionStatusMessage(AuctionStatusMessage* msg){
    if(msg->getMessageType() == (int)MessageType::NOTIFY_AUCTION){
        if(!messageCache.hasReceived(msg->getMessageId())){
            std::cout << myId <<": Got notified of auction!" << std::endl;
            messageCache.saveReceived(msg->getMessageId());
            int auctionId = msg->getAuctionId(), context = msg->getContext(), auctionManager = msg->getManagerId();
            Trigger notify(Belief("receive/auction"), myId, auctionId);
            notify.appendInt(context); notify.appendInt(auctionManager);
            manager->QueueTrigger(notify);
        }
        AuctionStatusMessage* reply = new AuctionStatusMessage("NotifyAuctionAck");
        fillNegotiationMessage(reply, myId, msg->getVehicleId(), false, msg->getMessageId());
        reply->setMessageType((int)MessageType::ACK);
        sendMessageDelayed(reply, msg->getVehicleId());
    }else if (msg->getMessageType() == (int)MessageType::ACK){
        messageCache.markReceived(msg->getReplyMessageId(), msg->getVehicleId());
    }else if (msg->getMessageType() == (int)MessageType::ITERATION_RESULTS){
        if(!messageCache.hasReceived(msg->getMessageId())){
            std::cout << myId <<": Got results of an iteration!" << std::endl;
            messageCache.saveReceived(msg->getMessageId());
            int status = msg->getWinnerId() == myId ? CONFIRMED : REJECTED;
            Trigger iterationResult (Belief("receive/round/result"), myId, status);
            manager->QueueTrigger(iterationResult);
        }
        AuctionStatusMessage* reply = new AuctionStatusMessage("NotifyAuctionAck");
        fillNegotiationMessage(reply, myId, msg->getVehicleId(), false, msg->getMessageId());
        reply->setMessageType((int)MessageType::ACK);
        sendMessageDelayed(reply, msg->getVehicleId());
    }else if (msg->getMessageType() == (int)MessageType::AUCTION_END){
        if(!messageCache.hasReceived(msg->getMessageId())){
            std::cout << myId <<": Got results of the auction!" << std::endl;
            messageCache.saveReceived(msg->getMessageId());
            int status = msg->getWinnerId() == myId ? CONFIRMED : REJECTED;
            int duePayment = msg->getDuePayment();
            Trigger auctionResult(Belief("receive/result"), myId, status);
            auctionResult.appendInt(duePayment);
            manager->QueueTrigger(auctionResult);
        }
        AuctionStatusMessage* reply = new AuctionStatusMessage("AuctionEndAck");
        fillNegotiationMessage(reply, myId, msg->getVehicleId(), false, msg->getMessageId());
        reply->setMessageType((int)MessageType::ACK);
        sendMessageDelayed(reply, msg->getVehicleId());
    }else if (msg->getMessageType() == (int)MessageType::DISTRIBUTION){
        if(!messageCache.hasReceived(msg->getMessageId())){
            messageCache.saveReceived(msg->getMessageId());
            if(myId != msg->getWinnerId()){
                std::cout << myId <<": Got results the payment of auction!" << std::endl;
                double percentage;
                int pay;
                if(msg->getContext() == CONTEXT_JOIN){
                    if(msg->getPlatoonId() == positionHelper->getPlatoonId()){
                        int platoonPosition = ((DynamicPositionHelper*)positionHelper)->getPosition();
                        if(platoonPosition == 0)
                            percentage = 2;
                        else if(platoonPosition == 1)
                            percentage = 4;
                        else
                            percentage = 4*(positionHelper->getPlatoonSize() - 2);
                        pay = (int)(msg->getPayment() / percentage);
                    }else{
                        pay = 0;
                    }
                }else{
                    percentage = (double)willingnessToPay / msg->getWtpsum();
                    pay = (int)(msg->getPayment() * percentage);
                }
                int speed;
                std::vector<int> path;
                Trigger auctionResult(Belief("receive/pay"), myId, pay);
                if(msg->getContext() == CONTEXT_SPEED){
                    speed = msg->getProperty();
                    auctionResult.appendInt(speed);
                    ((MarketManager*)manager)->timeStampSpeed(simTime().dbl() * 1000);
                }else if(msg->getContext() == CONTEXT_PATH){
                    path = msg->getPropertyList();
                    auctionResult.appendVector(path);
                    ((MarketManager*)manager)->timeStampRoute(simTime().dbl() * 1000);
                }else{
                    speed = msg->getProperty();
                    auctionResult.appendInt(speed);
                }
                manager->QueueTrigger(auctionResult);
            }
        }
        AuctionStatusMessage* reply = new AuctionStatusMessage("DistributionAck");
        fillNegotiationMessage(reply, myId, msg->getVehicleId(), false, msg->getMessageId());
        reply->setMessageType((int)MessageType::ACK);
        sendMessageDelayed(reply, msg->getVehicleId());
    }
}

void MarketAgent::handleEndOfAuction(int auctionId, int auctionIteration, int winnerId, int duePayment){
    Enter_Method_Silent();
    AuctionStatusMessage* endAuction = new AuctionStatusMessage("EndOfAuction");
    fillNegotiationMessage(endAuction, myId, -1, true);
    endAuction->setMessageType((int)MessageType::AUCTION_END);
    endAuction->setAuctionId(auctionId);
    endAuction->setManagerId(myId);
    endAuction->setWinnerId(winnerId);
    endAuction->setAuctionIteration(auctionIteration);
    endAuction->setDuePayment(duePayment);
    sendMessageWithAck(endAuction, auctionMembers);
}
void MarketAgent::handleEndOfAuction(int auctionId, int auctionIteration, int winnerId, int pay, int wtpSum, int context){
    Enter_Method_Silent();
    if(context == CONTEXT_SPEED){
        distributePay(auctionId, auctionIteration, winnerId, pay, wtpSum, ((MarketManager*)manager)->getPreferredSpeed(myId));
    }else if(context == CONTEXT_PATH){
        distributePay(auctionId, auctionIteration, winnerId, pay, wtpSum, ((MarketManager*)manager)->getPreferredPath(myId));
    }
}

void MarketAgent::sendPay(int auctionId, int context, int pay, int managerId, int speed){
    Enter_Method_Silent();
    BidMessage* payment = new BidMessage("payment");
    fillNegotiationMessage(payment, myId, managerId, false);
    payment->setMessageType((int)MessageType::PAYMENT);
    payment->setAuctionId(auctionId);
    payment->setContext(context);
    payment->setBidValue(pay);
    payment->setWtp(willingnessToPay);
    payment->setProperty(((MarketManager*)manager)->getPreferredSpeed(myId));
    sendMessageWithAck(payment, managerId);
}
void MarketAgent::sendPay(int auctionId, int context, int pay, int managerId, std::vector<int> route){
    Enter_Method_Silent();
    BidMessage* payment = new BidMessage("payment");
    fillNegotiationMessage(payment, myId, managerId, false);
    payment->setMessageType((int)MessageType::PAYMENT);
    payment->setAuctionId(auctionId);
    payment->setContext(context);
    payment->setBidValue(pay);
    payment->setWtp(willingnessToPay);
    payment->setPropertyList(((MarketManager*)manager)->getPreferredPath(myId));
    sendMessageWithAck(payment, managerId);
}

void MarketAgent::distributePay(int auctionId, int auctionIteration, int winnerId, int payment, int wtpSum, int speed){
    Enter_Method_Silent();
    AuctionStatusMessage* distributePay = new AuctionStatusMessage("distributePay");
    fillNegotiationMessage(distributePay, myId, -1, true);
    distributePay->setMessageType((int)MessageType::DISTRIBUTION);
    distributePay->setAuctionId(auctionId);
    distributePay->setManagerId(myId);
    distributePay->setWinnerId(winnerId);
    distributePay->setAuctionIteration(auctionIteration);
    distributePay->setWtpsum(wtpSum);
    distributePay->setPayment(payment);
    distributePay->setProperty(speed);
    distributePay->setContext(CONTEXT_SPEED);
    sendMessageWithAck(distributePay, auctionMembers);
    endOfAuctionTrigger(winnerId);
}
void MarketAgent::distributePay(int auctionId, int auctionIteration, int winnerId, int payment, int wtpSum, std::vector<int> route){
    Enter_Method_Silent();
    AuctionStatusMessage* distributePay = new AuctionStatusMessage("distributePay");
    fillNegotiationMessage(distributePay, myId, -1, true);
    distributePay->setMessageType((int)MessageType::DISTRIBUTION);
    distributePay->setAuctionId(auctionId);
    distributePay->setManagerId(myId);
    distributePay->setWinnerId(winnerId);
    distributePay->setAuctionIteration(auctionIteration);
    distributePay->setWtpsum(wtpSum);
    distributePay->setPayment(payment);
    distributePay->setPropertyList(route);
    distributePay->setContext(CONTEXT_PATH);
    sendMessageWithAck(distributePay, auctionMembers);
    endOfAuctionTrigger(winnerId);
}


void MarketAgent::delegateNegotiationMessage(NegotiationMessage* nm){
    if(AuctionStatusMessage* msg = dynamic_cast<AuctionStatusMessage*>(nm)){
        if(isReceiver(msg)) handleAuctionStatusMessage(msg);
        delete msg;
    }else if(BidMessage* msg = dynamic_cast<BidMessage*>(nm)){
        if(isReceiver(msg)) handleBidMessage(msg);
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
            ((MarketManager*)manager)->incrementRetransmission();
        }
    }else if(msg == auctionTrigger){
        Trigger startAuction(Belief("start/auction"), myId, atc.context);
        if(atc.context == CONTEXT_SPEED){
            Trigger wtp(Belief("receive/wtp"), myId, wtpList);
            manager->QueueTrigger(wtp);
        }else if(atc.context == CONTEXT_JOIN){
            ((MarketManager*)manager)->startTimeStampJoin(simTime().dbl() * 1000);
        }
        startAuction.appendVector(auctionMembers);
        manager->QueueTrigger(startAuction);
    }else if(MarketMessage* delayMessage = dynamic_cast<MarketMessage*>(msg)){
        switch(delayMessage->getMessageType()){
        case (int)MessageType::ACK:
            sendUnicast(delayMessage, delayMessage->getDestinationId());
            break;
        default:
            sendMessageWithAck(delayMessage, delayMessage->getDestinationId());
            break;
        }
    }else{
        GeneralPlexeAgentAppl::handleSelfMsg(msg);
    }
}

void MarketAgent::endOfAuctionTrigger(int winnerId){
    if(atc.context == CONTEXT_SPEED){
        scheduleAt(simTime() + 1, auctionTrigger);
        atc.context = CONTEXT_PATH;
    }
}

