/*
 * RouteVotingAppl.h
 *
 *  Created on: 14/10/2018
 *      Author: miguel
 */

#ifndef APP_PLEXEAPPL_CONCRETE_ROUTEVOTINGAPPL_H_
#define APP_PLEXEAPPL_CONCRETE_ROUTEVOTINGAPPL_H_

#include "VotingAppl.h"

class RouteVotingAppl : public VotingAppl {
public:
    RouteVotingAppl(){};
    virtual ~RouteVotingAppl(){}
    /** override from the normal appl */
    virtual void initialize(int stage) override;
protected:
    virtual void handleLowerMsg(cMessage* msg) override;
};

#endif /* APP_PLEXEAPPL_CONCRETE_ROUTEVOTINGAPPL_H_ */
