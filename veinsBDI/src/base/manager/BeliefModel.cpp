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

