/*
 * RouterAppl.cpp
 *
 *  Created on: 18/11/2018
 *      Author: miguel
 */

#include "RouterAppl.h"

Define_Module(RouterAppl);

void RouterAppl::initialize(int stage){
    BasePlexeAgentAppl::initialize(stage);
    if(stage == 1){
        notifyNodeClose = new cMessage("notifyNodeClose");
        scheduleAt(simTime() + 0.3, notifyNodeClose);
    }
}

void RouterAppl::setTarget(std::vector<int> route){
    //"0,1 1,2 2,5 5,7"
    std::list<std::string> edges;
    convertToEdgesList(route, edges);
    /*edges.push_back("0,1");
    edges.push_back("1,2");
    edges.push_back("2,5");
    edges.push_back("5,7");*/
    traciVehicle->newRoute(roadId);
}

void RouterAppl::convertToEdgesList(std::vector<int>& route, std::list<std::string>& edges){
    for(uint32_t i = 0; i < route.size() - 1; i++){
        edges.push_back(std::to_string(route[i]) + "," + std::to_string(route[i+1]));
    }
}

void RouterAppl::handleSelfMsg(cMessage* msg)
{
    if(msg == notifyNodeClose){
        delete msg;
        BeliefModel b("link/closed");
        b.pushString("n1");
        b.pushString("n3");
        manager->sendInformationToAgents(myId, &b);
    }else{
        BasePlexeAgentAppl::handleSelfMsg(msg);
    }
}
