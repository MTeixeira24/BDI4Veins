/*
 * BeliefModel.h
 *
 *  Created on: 29/08/2018
 *      Author: miguel
 */

#ifndef BASE_MANAGER_BELIEFMODEL_H_
#define BASE_MANAGER_BELIEFMODEL_H_

#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include "LightJasonConstants.h"
#include "BeliefObject.h"

class BeliefModel {
public:
    BeliefModel();
    BeliefModel(std::string belief);
    virtual ~BeliefModel();
    void setBelief(std::string);
    std::string getBelief(){return belief;}
    int getTotalSize(){return totalSize;}

    void pushBool(bool*);
    void pushChar(char*);
    void pushInt(int*);
    void pushLong(long*);
    void pushFloat(float*);
    void pushDouble(double*);

    void pushIntArray(std::vector<int>&);
    void pushIntArray(int* array, uint32_t size);

    void pushDoubleArray(std::vector<double>&);

    std::vector<BeliefObject> getValues(){return values;}
protected:
    std::string belief;
    std::vector<BeliefObject> values;
    int totalSize;
};

#endif /* BASE_MANAGER_BELIEFMODEL_H_ */
