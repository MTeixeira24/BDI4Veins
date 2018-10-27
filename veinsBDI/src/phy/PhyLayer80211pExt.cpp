/*
 * PhyLayer80211pExt.cpp
 *
 *  Created on: 27/10/2018
 *      Author: miguel
 */

#include "PhyLayer80211pExt.h"


Define_Module(PhyLayer80211pExt);

Veins::AnalogueModel* PhyLayer80211pExt::getAnalogueModelFromName(std::string name, ParameterMap& params)
{
    if (name == "NakagamiFading") {
        return initializeNakagamiFadingImproved(params);
    }
    return PhyLayer80211p::getAnalogueModelFromName(name, params);
}

Veins::AnalogueModel* PhyLayer80211pExt::initializeNakagamiFadingImproved(ParameterMap& params)
{
    bool constM = params["constM"].boolValue();
    double m = 0;
    if (constM) {
        m = params["m"].doubleValue();
    }
    return new Veins::NakagamiFadingImproved(constM, m, coreDebug);
}
