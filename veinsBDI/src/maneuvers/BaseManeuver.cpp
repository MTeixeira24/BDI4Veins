/*
 * BaseManeuver.cpp
 *
 *  Created on: 10/09/2018
 *      Author: miguel
 */

#include "BaseManeuver.h"
#include "../app/plexeappl/GeneralPlexeAgentAppl.h"

BaseManeuver::BaseManeuver(GeneralPlexeAgentAppl* app) {
    this->app = app;
    this->positionHelper = app->getPositionHelper();
    this->mobility = app->getMobility();
    this->traci = app->getTraci();
    this->traciVehicle = app->getTraciVehicle();
}

