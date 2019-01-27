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
    if(atc.context == CONTEXT_SPEED){
        MarketAgent::endOfAuctionTrigger(winnerId);
    }else if(!hasJoinEnded){
        if(atc.context == CONTEXT_PATH){
            cancelEvent(auctionTrigger);
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
            scheduleAt(simTime() + 1, auctionTrigger);
            atc.context = CONTEXT_SPEED;
            auctionMembers.clear();
            wtpList.clear();
            auctionSize = positionHelper->getPlatoonSize();
            agentBidTuples.reserve(auctionSize*2);
            wtpList.reserve(positionHelper->getPlatoonSize() + 1);
            std::vector<int> members = positionHelper->getPlatoonFormation();
            members.push_back(winnerId);
            auctionMembers.insert(auctionMembers.begin(), members.begin(), members.end());
            for(uint32_t i = 0; i < members.size(); i++)
                wtpList.push_back(((MarketManager*)manager)->getWTP(members[i]));
            hasJoinEnded = true;
            ((MarketManager*)manager)->endTimeStampJoin(simTime().dbl() * 1000);
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
        distributePay->setPayment(payment);
        distributePay->setProperty(speed);
        distributePay->setContext(CONTEXT_JOIN);
        distributePay->setPlatoonId(positionHelper->getPlatoonId());

        std::vector<int> targets = auctionMembers;
        std::vector<int> platoons = positionHelper->getPlatoonFormation();
        targets.insert(targets.end(), platoons.begin() + 1, platoons.end());

        sendMessageWithAck(distributePay, targets);
        endOfAuctionTrigger(winnerId);
    }else{
        MarketAgent::distributePay(auctionId, auctionIteration, winnerId, payment, wtpSum, speed);
    }
}

bool MarketJoinerAgent::isReceiver(MarketMessage* msg){
    if(msg->getTargets().find(myId) != msg->getTargets().end())
        return true;
    if(msg->getDestinationId() == myId)
        return true;
    return false;
}
