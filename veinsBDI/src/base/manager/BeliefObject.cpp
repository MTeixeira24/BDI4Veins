/*
 * BeliefObject.cpp
 *
 *  Created on: 29/08/2018
 *      Author: miguel
 */

#include "BeliefObject.h"

BeliefObject::BeliefObject() {

}

BeliefObject::~BeliefObject() {
}

void BeliefObject::setData(uint16_t _dataType, void* _data, uint32_t _size){
    dataType = _dataType;
    data = _data;
    size = _size;
}

