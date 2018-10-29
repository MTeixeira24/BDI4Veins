/*
 * JoinScenario.cpp
 *
 *  Created on: Sep 7, 2018
 *      Author: miguel
 */

#include "JoinScenario.h"
#include "veins/modules/application/platooning/utilities/DynamicPositionManager.h"

Define_Module(JoinScenario);


void JoinScenario::initialize(int stage)
{

    BaseScenario::initialize(stage);

    if (stage == 0){
        // get pointer to application
        app = FindModule<GeneralPlexeAgentAppl*>::findSubModule(getParentModule());
    }

    if (stage == 1) {

        /*CHECK FOR NON PLATOONING VEHICLES*/
        DynamicPositionManager& positions = DynamicPositionManager::getInstance();
        int aux = positions.vehToPlatoons.count(positionHelper->getId());
        if(aux == 0) {
            traciVehicle->setFixedLane(traciVehicle->getLaneIndex(), false);
            traciVehicle->setActiveController(Plexe::ACC);
            //To faciliate maneuvering and speed up simulation, have the joiner
            //travel at a constant speed
            traciVehicle->setSpeed(mobility->getSpeed() - 1);
            //traciVehicle->setSpeedMode(0);
            traciVehicle->setCruiseControlDesiredSpeed(mobility->getSpeed() + 2.88);
            positionHelper->setPlatoonId(-1);
            positionHelper->setIsLeader(false);
            positionHelper->setPlatoonLane(-1);
       }else{
           double speedModifier[] =  {1, 0.6, 1.5};
           int platoonId = positionHelper->getPlatoonId();
           //traciVehicle->setSpeed(mobility->getSpeed() * speedModifier[platoonId]);
           double modSpeed = mobility->getSpeed() * speedModifier[platoonId];

           platooningVType = par("platooningVType").stdstringValue();

           traciVehicle->setFixedLane(traciVehicle->getLaneIndex(), false);
           traciVehicle->setSpeedMode(0);
           if (positionHelper->isLeader()) {
               for (int i = 1; i < positionHelper->getPlatoonSize(); i++) {
                   std::stringstream ss;
                   ss << platooningVType << "." << positionHelper->getMemberId(i);
                   traciVehicle->addPlatoonMember(ss.str(), i);
               }
               traciVehicle->enableAutoLaneChanging(false);
               traciVehicle->setFixedLane(traciVehicle->getLaneIndex(), true);
               traciVehicle->setCruiseControlDesiredSpeed(modSpeed);
           }
           else {
               std::stringstream ssl, ss;
               ssl << platooningVType << "." << positionHelper->getLeaderId();
               ss << platooningVType << "." << positionHelper->getFrontId();
               //traciVehicle->enableAutoFeed(false, ssl.str(), ss.str());
               traciVehicle->setCruiseControlDesiredSpeed(mobility->getSpeed());
           }
           positionHelper->setPlatoonSpeed(modSpeed);
       }
    }
}

void JoinScenario::handleSelfMsg(cMessage* msg)
{

    // this takes care of feeding data into CACC and reschedule the self message
    BaseScenario::handleSelfMsg(msg);
}
