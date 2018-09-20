/*
 * BeliefModel.cpp
 *
 *  Created on: 29/08/2018
 *      Author: miguel
 */

#include "BeliefModel.h"

BeliefModel::BeliefModel() {
    totalSize = 0;
}

BeliefModel::BeliefModel(std::string _belief) {
    totalSize = 0;
    belief = _belief;
}

BeliefModel::~BeliefModel() {
}

void BeliefModel::setBelief(std::string _belief){
    belief = _belief;
}

void BeliefModel::pushInt(int* value){
    BeliefObject bo;
    bo.setData(VALUE_INT, value, 4);
    values.push_back(bo);
    totalSize += 4;
}

void BeliefModel::pushDouble(double* value){
    BeliefObject bo;
    bo.setData(VALUE_DOUBLE, value, 8);
    values.push_back(bo);
    totalSize += 8;
}

void BeliefModel::pushIntArray(std::vector<int>& elements){
    BeliefObject bo;
    size_t size = sizeof(int)*elements.size();
    short* array = (short*)malloc(sizeof(short)+size); //2 bytes for type specifier, 4 byte sequence for int
    *(array + sizeof(short)) = VALUE_INT; //Put a 2 byte-sized type specifier in the message;
    for(uint32_t i = 0; i < elements.size(); i++){
        *( (array + sizeof(short)) + sizeof(int)*i ) = elements[i];
    }
    bo.setData(VALUE_ARRAY, array, size);
    totalSize += size;
}

