/*
 * RouterManager.h
 *
 *  Created on: 18/11/2018
 *      Author: miguel
 */

#ifndef BASE_ROUTERMANAGER_ROUTERMANAGER_H_
#define BASE_ROUTERMANAGER_ROUTERMANAGER_H_

#include "../manager/LightJasonManager.h"
#include "../../app/plexeappl/concrete/router/RouterAppl.h"

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

#endif /* BASE_ROUTERMANAGER_ROUTERMANAGER_H_ */
