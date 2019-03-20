/*
 * Trigger.cpp
 *
 *  Created on: 20/03/2019
 *      Author: miguel
 */

#include "Trigger.h"


Trigger::Trigger(uint8_t triggerId, uint32_t agentId){
    initialize(triggerId, agentId, NULL, 0, DataType::NONE);
}

Trigger::Trigger(uint8_t triggerId, uint32_t agentId, int parameter){
    initialize(triggerId, agentId, &parameter, 1, DataType::NUMBER);
}

Trigger::Trigger(uint8_t triggerId, uint32_t agentId, std::string parameter){
    const char *data = parameter.c_str();
    initialize(triggerId, agentId, data, parameter.length(), DataType::STRING);
}

Trigger::Trigger(uint8_t triggerId, uint32_t agentId, std::vector<int> parameter){
    initialize(triggerId, agentId, parameter.data(), parameter.size(), DataType::NUMBER_SEQUENCE);
}

void Trigger::initialize(uint8_t triggerId, uint32_t agentId,const void* parameter, uint32_t length, DataType type){
    triggerHead = new TriggerSequence;
    this->triggerId = triggerId;
    this->agentId = agentId;
    triggerHead->next = NULL;
    triggerHead->type = type;
    triggerHead->dataLength = length;

    switch(type){
    case DataType::NUMBER:
        triggerHead->dataSequence = new int;
        *((int*)(triggerHead->dataSequence)) = *((const int*)parameter);
        break;
    case DataType::NUMBER_SEQUENCE:
        triggerHead->dataSequence = malloc(length*sizeof(int));
        memcpy(triggerHead->dataSequence, parameter, length*sizeof(int));
        break;
    case DataType::STRING:
        triggerHead->dataSequence  = malloc(length*sizeof(char));
        memcpy(triggerHead->dataSequence, parameter, length);
        break;
    default:
        break;
    }

    triggerTail=triggerHead;
}

Trigger::~Trigger() {
    deleteTriggerList(triggerHead);
}

void Trigger::deleteTriggerList(TriggerSequence* trigger){
    if(trigger->next != NULL){
        deleteTriggerList(trigger->next);
    }
    delete trigger;
}

void Trigger::appendParameter(uint32_t length, DataType type){
    TriggerSequence* newParam = new TriggerSequence;

    newParam->next = NULL;
    newParam->type = type;
    newParam->dataLength = length;

    triggerTail->next = newParam;
    triggerTail = newParam;
}

void Trigger::appendInt(int parameter){
    appendParameter(1, DataType::NUMBER);
    triggerTail->dataSequence = new int;
    *((int*)(triggerTail->dataSequence)) = parameter;
}

void Trigger::appendString(std::string parameter){
    appendParameter(parameter.size(), DataType::STRING);
    triggerTail->dataSequence  = malloc(parameter.size()*sizeof(char));
    memcpy(triggerTail->dataSequence, parameter.c_str(), parameter.size()*sizeof(char));
}

void Trigger::appendVector(std::vector<int> parameter){
    appendParameter(parameter.size(), DataType::NUMBER_SEQUENCE);
    triggerTail->dataSequence = malloc(parameter.size()*sizeof(int));
    memcpy(triggerTail->dataSequence, parameter.data(), parameter.size()*sizeof(int));
}

void Trigger::convertToMessage(LightJasonBuffer& queue){
    //write the belief id and agent id
    queue <<  agentId << triggerId;
    //startWritting the params
    TriggerSequence* node = triggerHead;
    while(node != NULL){
        //write the data identifier
        queue << node->type;

        //If a sequence, write the data length to read
        if((node->type == DataType::NUMBER_SEQUENCE) || (node->type == DataType::STRING))
            queue << node->dataLength;

        //Finally put the data in
        for(uint8_t i = 0; i < node->dataLength; i++){
            if(node->type == DataType::NUMBER_SEQUENCE || node->type == DataType::NUMBER)
                queue << ((int*)(node->dataSequence))[i];
            else
                queue << ((char*)(node->dataSequence))[i];
        }

        //append the end of parameter identifier

        node = node->next;
        if(node != NULL) queue << 0xFFFD;
    }
    //append the end of trigger identifier
    queue << 0xFFFE;
}
