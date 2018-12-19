/*
 * MessageCache.cpp
 *
 *  Created on: Dec 18, 2018
 *      Author: miguel
 */

#include "MessageCache.h"

MessageCache::MessageCache() {}

MessageCache::~MessageCache() {}


bool MessageCache::allResponded(long msgId){
    bool status = true;
    if(existsEntry(msgId)){
        MessageStatus* ms = messageCacheMap[msgId];
        ms->remainders.clear();
        for(unsigned int i = 0; i < ms->receiverIds.size(); i++){
            if(!ms->receiverStatus[ms->receiverIds[i]]) ms->remainders.insert(ms->receiverIds[i]);
            status &= ms->receiverStatus[i];
        }
    }
    return status;
}

void MessageCache::insertEntry(long msgId, MarketMessage* msgPointer, const std::vector<int>& ids){
    deleteEntry(msgId);
    MessageStatus* ms = new MessageStatus;
    ms->message_backup = msgPointer;
    ms->receiverIds = std::vector<int>(ids);
    ms->receiverStatus = std::unordered_map<int, bool>();
    for(unsigned int i = 0; i < ids.size(); i++)
        ms->receiverStatus[i] = false;
    if(ms->receiverStatus.find(senderId) != ms->receiverStatus.end())
        ms->receiverStatus[senderId] = true; //We don't expect the sender to send a reply to itself
    messageCacheMap[msgId] = ms;
}

void MessageCache::deleteEntry(long msgId){
    if(existsEntry(msgId)) messageCacheMap.erase(msgId);
}

bool MessageCache::existsEntry(long msgId){
    return messageCacheMap.find(msgId) != messageCacheMap.end();
}

MarketMessage* MessageCache::getMessageReference(long msgId){
    if(existsEntry(msgId)) return messageCacheMap[msgId]->message_backup;
    return NULL;
}

void MessageCache::markReceived(long msgId, int id){
    messageCacheMap[msgId]->receiverStatus[id] = true;
}
