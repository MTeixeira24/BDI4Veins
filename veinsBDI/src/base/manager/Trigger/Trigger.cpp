/*
 * Trigger.cpp
 *
 *  Created on: 20/03/2019
 *      Author: miguel
 */

#include "Trigger.h"


Trigger::Trigger(uint8_t triggerId){
    Trigger(triggerId, NULL, 0, DataType::NONE);
}

Trigger::Trigger(uint8_t triggerId, int parameter){
    int *data = new int;
    *data = parameter;
    Trigger(triggerId, data, 1, DataType::NUMBER);
}

Trigger::Trigger(uint8_t triggerId, std::string parameter){

    char *data = (char*)malloc(parameter.length()*sizeof(char));
    memcpy(data, parameter.c_str(), parameter.length());
    Trigger(triggerId, data, parameter.length(), DataType::STRING);
}

Trigger::Trigger(uint8_t triggerId, std::vector<int> parameter){
    int *data = (int*)malloc(parameter.size()*sizeof(int));
    memcpy(data, parameter.data(), parameter.size()*sizeof(int));
    Trigger(triggerId, data, parameter.size(), DataType::NUMBER_SEQUENCE);
}

Trigger::Trigger(uint8_t triggerId, void* parameter, uint32_t length, DataType type){
    this->triggerId = triggerId;
    triggerHead->next = NULL;
    triggerHead->type = type;
    triggerHead->dataSequence = parameter;
    triggerHead->dataLength = length;

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

void Trigger::appendParameter(void* parameter, uint32_t length, DataType type){
    TriggerSequence* newParam = new TriggerSequence;

    newParam->next = NULL;
    newParam->type = type;
    newParam->dataSequence = parameter;
    newParam->dataLength = length;

    triggerTail->next = newParam;
    triggerTail = newParam;
}

void Trigger::appendInt(int parameter){
    char *data = (char*)malloc(sizeof(int));
    memcpy(data, &parameter, sizeof(int));
    appendParameter(data, 1, DataType::NUMBER);
}

void Trigger::appendString(std::string parameter){
    char *data = (char*)malloc(parameter.length()*sizeof(char));
    memcpy(data, parameter.c_str(), parameter.length());
    appendParameter(data, parameter.size(), DataType::STRING);
}

void Trigger::appendVector(std::vector<int> parameter){
    char *data = (char*)malloc(parameter.size()*sizeof(int));
    memcpy(data, parameter.data(), parameter.size()*sizeof(int));
    appendParameter(data, parameter.size(), DataType::NUMBER_SEQUENCE);
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
        queue << 0xFFFD;

        node = node->next;
    }
    //append the end of trigger identifier
    queue << 0xFFFE;
}
