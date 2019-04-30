/*
 * BeliefModel.cpp
 *
 *  Created on: 29/08/2018
 *      Author: miguel
 */

#include "../../managers/manager/BeliefModel.h"

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

void BeliefModel::pushInt(int value){
    BeliefObject bo;
    bo.setData(VALUE_INT, &value, 4);
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
    size_t size = sizeof(int) * (elements.size()+2);
    int *array = new int[elements.size()+2];
    //int array[elements.size()+2]; //allocate size() bytes for content and 2 extra for type and size.
    array[0] = VALUE_INT; //Put the value type at beginning;
    array[1] = elements.size(); //store the size;
    for(uint32_t i = 0; i < elements.size(); i++){
        array[i + 2] = elements[i];
    }
    bo.setData(VALUE_ARRAY, array, size);
    values.push_back(bo);
    totalSize += size;
}

void BeliefModel::pushIntArray(int* array, uint32_t size){
    BeliefObject bo;
    size_t byte_size = sizeof(int) * (size + 2);
    int d_array[size + 2];
    d_array[0] = VALUE_INT;
    d_array[1] = size;
    std::memcpy( &d_array[2], array, sizeof(int)*size );
    bo.setData(VALUE_ARRAY, d_array, byte_size);
    values.push_back(bo);
    totalSize += byte_size;
}

void BeliefModel::pushDoubleArray(std::vector<double>& elements){
    BeliefObject bo;
    size_t size = sizeof(double) * (elements.size()+1);
    double *array = new double[elements.size()+1];
    //double array[elements.size()+1]; //allocate size() bytes for content and 2 extra for type and size.
    *( (int*)array ) = VALUE_DOUBLE;
    *( (int*)array + 1 ) = elements.size(); //store the size;
    for(uint32_t i = 0; i < elements.size(); i++){
        array[i + 1] = elements[i];
    }
    bo.setData(VALUE_ARRAY, array, size);
    values.push_back(bo);
    totalSize += size;
}

void BeliefModel::pushString(std::string str){
    BeliefObject bo;
    size_t size = str.size() + 8;
    char *array = new char[size];
    *( (int*)array ) = VALUE_CHAR;
    *( (int*)array + 1 ) = str.size();
    for(uint32_t i = 0; i < str.size(); i++){
            array[i + 9] = str[i];
    }
    bo.setData(VALUE_ARRAY, array, size);
    values.push_back(bo);
    totalSize += size;
}
