/*
 * PhyLayer80211pExt.h
 *
 *  Created on: 27/10/2018
 *      Author: miguel
 */

#ifndef PHY_PHYLAYER80211PEXT_H_
#define PHY_PHYLAYER80211PEXT_H_

#include "analogueModels/NakagamiFadingImproved.h"
#include "veins/modules/phy/PhyLayer80211p.h"

class PhyLayer80211pExt : public Veins::PhyLayer80211p {
public:
    PhyLayer80211pExt(){};
    virtual ~PhyLayer80211pExt(){};
protected:
    /**
     * @brief Creates and returns an instance of the AnalogueModel with the
     * specified name.
     */
    virtual Veins::AnalogueModel* getAnalogueModelFromName(std::string name, ParameterMap& params) override;
    Veins::AnalogueModel* initializeNakagamiFadingImproved(ParameterMap& params);
};

#endif /* PHY_PHYLAYER80211PEXT_H_ */
