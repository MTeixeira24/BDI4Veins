/*
 * BeliefObject.h
 *
 *  Created on: 29/08/2018
 *      Author: miguel
 */

#ifndef MANAGERS_MANAGER_BELIEFOBJECT_H_
#define MANAGERS_MANAGER_BELIEFOBJECT_H_

#include <cstdint>

class BeliefObject {
public:
    BeliefObject();
    virtual ~BeliefObject();

    void setData(uint16_t, void*, uint32_t);

    void* getData(){return data;}
    uint16_t getDataType(){return dataType;}
    uint32_t getSize(){return size;}

protected:
    void* data;
    uint16_t dataType;
    uint32_t size;
};

#endif /* MANAGERS_MANAGER_BELIEFOBJECT_H_ */
