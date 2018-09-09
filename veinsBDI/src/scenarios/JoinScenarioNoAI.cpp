/*
 * JoinScenarioNoAI.cpp
 *
 *  Created on: Sep 7, 2018
 *      Author: miguel
 */

#include "JoinScenarioNoAI.h"
#include "veins/modules/application/platooning/utilities/DynamicPositionManager.h"

Define_Module(JoinScenarioNoAI);

JoinScenarioNoAI::JoinScenarioNoAI() {

}

JoinScenarioNoAI::~JoinScenarioNoAI() {
}

void JoinScenarioNoAI::initialize(int stage)
{

    BaseScenario::initialize(stage);

    if (stage == 0){}
        // get pointer to application
        /*The joiner will be marked accordingly in its app: e.g. app.startsLone == true*/
        //app = FindModule<GeneralPlatooningApp*>::findSubModule(getParentModule());
        //appl = FindModule<BaseApp*>::findSubModule(getParentModule());

    if (stage == 1) {

        /*CHECK FOR NON PLATOONING VEHICLES*/
        DynamicPositionManager& positions = DynamicPositionManager::getInstance();
        int aux = positions.vehToPlatoons.count(positionHelper->getId());
        if(aux == 0) {
            traciVehicle->setFixedLane(traciVehicle->getLaneIndex(), false);
            traciVehicle->setActiveController(Plexe::ACC);
            traciVehicle->setCruiseControlDesiredSpeed(mobility->getSpeed());
            positionHelper->setPlatoonId(-1);
            positionHelper->setIsLeader(false);
            positionHelper->setPlatoonLane(-1);
       }else{
           double speedModifier[] =  {1, 0.6, 1.5};
           int platoonId = positionHelper->getPlatoonId();
           traciVehicle->setSpeed(mobility->getSpeed() * speedModifier[platoonId]);

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
               traciVehicle->setCruiseControlDesiredSpeed(mobility->getSpeed());
           }
           else {
               std::stringstream ssl, ss;
               ssl << platooningVType << "." << positionHelper->getLeaderId();
               ss << platooningVType << "." << positionHelper->getFrontId();
               //traciVehicle->enableAutoFeed(true, ssl.str(), ss.str());
               traciVehicle->setCruiseControlDesiredSpeed(mobility->getSpeed() + 10);
           }
       }
    }
}
