//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "LaneMergeScenario.h"

Define_Module(LaneMergeScenario);

LaneMergeScenario::LaneMergeScenario() {

}


LaneMergeScenario::~LaneMergeScenario() {
}


void LaneMergeScenario::initialize(int stage)
{
    BaseScenario::initialize(stage);

    if(stage == 0){
        app = FindModule<LaneMergeAgent*>::findSubModule(getParentModule());
    }
    if(stage == 1){
    }

}

void LaneMergeScenario::setup(){
    app->getTraciVehicle()->setSpeed(app->getMobility()->getSpeed());
    app->getTraciVehicle()->setFixedLane(app->getTraciVehicle()->getLaneIndex(), false);
}
