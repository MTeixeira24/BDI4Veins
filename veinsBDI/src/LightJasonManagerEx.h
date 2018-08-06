/*
 * LightJasonManagerEx.h
 *
 *  Created on: Aug 3, 2018
 *      Author: miguel
 */

#ifndef LIGHTJASONMANAGEREX_H_
#define LIGHTJASONMANAGEREX_H_


#include <omnetpp.h>

#include "veins/modules/mobility/traci/TraCIScenarioManagerLaunchd.h"


class LightJasonManagerEx : public Veins::TraCIScenarioManagerLaunchd
{
    public:

        virtual ~LightJasonManagerEx();
        virtual void initialize(int stage);
        virtual void finish();

    protected:
        void addModule(std::string nodeId, std::string type, std::string name, std::string displayString, const Coord& position, std::string road_id = "", double speed = -1, double angle = -1, VehicleSignal signals = VehicleSignal::VEH_SIGNAL_UNDEF);
        virtual void init_traci();
};


#endif /* LIGHTJASONMANAGEREX_H_ */
