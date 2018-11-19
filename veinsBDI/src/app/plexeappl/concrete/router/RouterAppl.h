/*
 * RouterAppl.h
 *
 *  Created on: 18/11/2018
 *      Author: miguel
 */

#ifndef APP_PLEXEAPPL_CONCRETE_ROUTER_ROUTERAPPL_H_
#define APP_PLEXEAPPL_CONCRETE_ROUTER_ROUTERAPPL_H_

#include "../../BasePlexeAgentAppl.h"

class RouterAppl : public BasePlexeAgentAppl{
public:
    RouterAppl(){};
    virtual ~RouterAppl(){};
    virtual void initialize(int stage) override;
    /*
     * Set the next node to travel
     */
    void setTarget(std::vector<int> route);
protected:
    virtual void handleSelfMsg(cMessage* msg) override;
private:
    cMessage* notifyNodeClose;
    void convertToEdgesList(std::vector<int>& route, std::list<std::string>& edges);
    std::string n1;
    std::string n2;
};

#endif /* APP_PLEXEAPPL_CONCRETE_ROUTER_ROUTERAPPL_H_ */
