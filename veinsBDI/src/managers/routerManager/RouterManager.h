/*
 * RouterManager.h
 *
 *  Created on: 18/11/2018
 *      Author: miguel
 */

#ifndef MANAGERS_ROUTERMANAGER_ROUTERMANAGER_H_
#define MANAGERS_ROUTERMANAGER_ROUTERMANAGER_H_

#include "../../app/router/RouterAppl.h"
#include "../../managers/manager/LightJasonManager.h"

class RouterManager : public LightJasonManager {
public:
    RouterManager(){};
    virtual ~RouterManager(){}
    void initialize(int) override;
protected:
    void parseResponse(uint32_t msgLength) override;
private:
    static constexpr uint32_t SET_NODE = 0x09;
};

#endif /* MANAGERS_ROUTERMANAGER_ROUTERMANAGER_H_ */
