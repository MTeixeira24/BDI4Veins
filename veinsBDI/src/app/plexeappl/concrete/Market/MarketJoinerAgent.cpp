/*
 * MarketJoinerAgent.cpp
 *
 *  Created on: Dec 30, 2018
 *      Author: miguel
 */

#include "MarketJoinerAgent.h"

Define_Module(MarketJoinerAgent)

MarketJoinerAgent::~MarketJoinerAgent() {
}


void MarketJoinerAgent::leaderBehaviour(){
    if(myId == 0)
        MarketAgent::leaderBehaviour();
}

void MarketJoinerAgent::endOfAuctionTrigger(int winnerId){
    MarketAgent::endOfAuctionTrigger(winnerId);
    if(!hasJoinEnded){
        if(atc.context == CONTEXT_PATH){
            delete auctionTrigger;
            auctionTrigger = new cMessage("auctionTriggerAuction");
            scheduleAt(simTime() + 1, auctionTrigger);
            atc.context = CONTEXT_JOIN;
            //We only care about three
            auctionSize = 3;
            agentBidTuples.reserve(auctionSize*2);
            wtpList.clear();
            auctionMembers.clear();
            int start = positionHelper->getPlatoonSize();
            for(int i = start; i < start + 3; i++){
                wtpList.push_back(((MarketManager*)manager)->getWTP(i));
                auctionMembers.push_back(i);
            }
        }else if(atc.context == CONTEXT_JOIN){
            delete auctionTrigger;
            auctionMembers.clear();
            wtpList.clear();
            MarketAgent::leaderBehaviour();
            wtpList.push_back(((MarketManager*)manager)->getWTP(winnerId));
            auctionMembers.push_back(winnerId);
        }
    }
}

void MarketJoinerAgent::distributePay(int auctionId, int auctionIteration, int winnerId, int payment, int wtpSum, int speed){
    if(atc.context == CONTEXT_JOIN){
        Enter_Method_Silent();
        AuctionStatusMessage* distributePay = new AuctionStatusMessage("distributePayJoin");
        fillNegotiationMessage(distributePay, myId, -1, true);
        distributePay->setMessageType((int)MessageType::DISTRIBUTION);
        distributePay->setAuctionId(auctionId);
        distributePay->setManagerId(myId);
        distributePay->setWinnerId(winnerId);
        distributePay->setAuctionIteration(auctionIteration);
        distributePay->setWtpsum(wtpSum);
        distributePay->setPayment(0);
        distributePay->setProperty(speed);
        distributePay->setContext(CONTEXT_JOIN);
        sendMessageWithAck(distributePay, auctionMembers);
    }else{
        MarketAgent::distributePay(auctionId, auctionIteration, winnerId, payment, wtpSum, speed);
    }
    endOfAuctionTrigger(winnerId);
}
