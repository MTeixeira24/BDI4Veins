/*
 * MessageCache.h
 *
 *  Created on: Dec 18, 2018
 *      Author: miguel
 */

#ifndef MESSAGES_MESSAGECACHE_H_
#define MESSAGES_MESSAGECACHE_H_

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>

#include "../market/MarketMessage_m.h"

struct MessageStatus{
    MarketMessage* message_backup;
    std::vector<int> receiverIds;
    std::unordered_map<int, bool> receiverStatus;
    std::unordered_set<int> remainders;
};

typedef std::unordered_map<long, MessageStatus*> MessageCacheMap;

/*
 * Defines attributes and methods to handle proper receipt of messages
 */
class MessageCache {
public:
    MessageCache();
    virtual ~MessageCache();
private:
    MessageCacheMap messageCacheMap;
    std::unordered_set<long> receivedMessages;
    int senderId;
public:
    bool allResponded(long msgId);
    std::unordered_set<int>& getRemainerIds(long msgId){return messageCacheMap[msgId]->remainders;};
    void insertEntry(long msgId, MarketMessage* msgPointer, const std::vector<int>& ids);
    void deleteEntry(long msgId);
    bool existsEntry(long msgId);
    MarketMessage* getMessageReference(long msgId);
    void markReceived(long msgId, int id);
    void setSenderId(int _senderId){senderId = _senderId;}
    void saveReceived(long id){receivedMessages.insert(id);}
    bool hasReceived(long id){return receivedMessages.find(id) != receivedMessages.end();}
};

#endif /* MESSAGES_MESSAGECACHE_H_ */
