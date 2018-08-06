/*
 * LightJasonManagerEx.cc
 *
 *  Created on: Aug 3, 2018
 *      Author: miguel
 */

#include "LightJasonManagerEx.h"

Define_Module(LightJasonManagerEx);

LightJasonManagerEx::~LightJasonManagerEx(){

}

void LightJasonManagerEx::initialize(int stage){
    TraCIScenarioManagerLaunchd::initialize(stage);
}

void LightJasonManagerEx::finish(){
    TraCIScenarioManagerLaunchd::finish();
}

void LightJasonManagerEx::addModule(std::string nodeId, std::string type, std::string name, std::string displayString, const Coord& position, std::string road_id, double speed, double angle, VehicleSignal signals){
    TraCIScenarioManager::addModule(nodeId, type, name, displayString, position, road_id, speed, angle, signals);
}

void LightJasonManagerEx::init_traci(){
    TraCIScenarioManagerLaunchd::init_traci();
}
