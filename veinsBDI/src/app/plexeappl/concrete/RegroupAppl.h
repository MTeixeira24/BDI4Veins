/*
 * RegroupAppl.h
 *
 *  Created on: Nov 14, 2018
 *      Author: miguel
 */

#ifndef APP_PLEXEAPPL_CONCRETE_REGROUPAPPL_H_
#define APP_PLEXEAPPL_CONCRETE_REGROUPAPPL_H_

#include "RouteVotingAppl.h"

class RegroupAppl : public RouteVotingAppl {
public:
    RegroupAppl();
    virtual ~RegroupAppl();
protected:
    virtual void leaderInitialBehaviour() override;
};

#endif /* APP_PLEXEAPPL_CONCRETE_REGROUPAPPL_H_ */
