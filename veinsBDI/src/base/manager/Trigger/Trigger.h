/*
 * Trigger.h
 *
 *  Created on: 20/03/2019
 *      Author: miguel
 */

#ifndef BASE_MANAGER_TRIGGER_TRIGGER_H_
#define BASE_MANAGER_TRIGGER_TRIGGER_H_

#include "../LightJasonBuffer.h"
using namespace Jason;


class Trigger {
public:
    Trigger(uint8_t triggerId, uint32_t agentId);
    Trigger(uint8_t triggerId, uint32_t agentId, int parameter);
    Trigger(uint8_t triggerId, uint32_t agentId, std::string parameter);
    Trigger(uint8_t triggerId, uint32_t agentId, std::vector<int> parameter);

    virtual ~Trigger();

    enum DataType : uint16_t{
        NONE,
        NUMBER,
        STRING,
        NUMBER_SEQUENCE
    };

private:

    void initialize(uint8_t triggerId, uint32_t agentId, const void* parameter, uint32_t length, DataType type);

    typedef struct _trigger_sequence{
        struct _trigger_sequence *next;
        DataType type;
        uint8_t dataLength;
        void *dataSequence;
    } TriggerSequence;

    TriggerSequence* triggerHead;
    TriggerSequence* triggerTail = NULL;

    uint32_t triggerId;
    uint32_t agentId;

public:
    void convertToMessage(LightJasonBuffer&);
    void appendInt(int parameter);
    void appendString(std::string parameter);
    void appendVector(std::vector<int> parameter);

    void appendParameter(uint32_t length, DataType type);
    void deleteTriggerList(TriggerSequence* trigger);
};

#endif /* BASE_MANAGER_TRIGGER_TRIGGER_H_ */
