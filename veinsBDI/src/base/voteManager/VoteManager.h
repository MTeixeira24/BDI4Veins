/*
 * VoteManager.h
 *
 *  Created on: 12/09/2018
 *      Author: miguel
 */

#ifndef BASE_VOTEMANAGER_VOTEMANAGER_H_
#define BASE_VOTEMANAGER_VOTEMANAGER_H_

#include "../manager/LightJasonManager.h"
#include "../../app/plexeappl/example/VotingAppl.h"

class VoteManager : public LightJasonManager {
public:
    VoteManager();
    virtual ~VoteManager();
    void initialize(int) override;
protected:
    void parseResponse(uint32_t msgLength) override;
};

#endif /* BASE_VOTEMANAGER_VOTEMANAGER_H_ */
