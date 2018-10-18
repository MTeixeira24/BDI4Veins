/*
 * InterferanceAppl.h
 *
 *  Created on: 18/10/2018
 *      Author: miguel
 */

#ifndef APP_INTERFERANCE_INTERFERANCEAPPL_H_
#define APP_INTERFERANCE_INTERFERANCEAPPL_H_

#include "veins/modules/application/platooning/apps/BaseApp.h"
#include "protocols/InterferingBeaconing.h"

class InterferanceAppl : public BaseApp {
public:
    InterferanceAppl(){};
    virtual ~InterferanceAppl(){};
    virtual void initialize(int stage) override;
};

#endif /* APP_INTERFERANCE_INTERFERANCEAPPL_H_ */
