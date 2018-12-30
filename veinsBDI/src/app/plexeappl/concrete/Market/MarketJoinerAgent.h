/*
 * MarketJoinerAgent.h
 *
 *  Created on: Dec 30, 2018
 *      Author: miguel
 */

#ifndef APP_PLEXEAPPL_CONCRETE_MARKET_MARKETJOINERAGENT_H_
#define APP_PLEXEAPPL_CONCRETE_MARKET_MARKETJOINERAGENT_H_

#include "MarketAgent.h"

class MarketJoinerAgent : public MarketAgent {
public:
    MarketJoinerAgent() : hasJoinEnded(false){};
    virtual ~MarketJoinerAgent();
protected:
    /*
     * Initial behaviours
     */
    virtual void leaderBehaviour() override;
    /*
     * end of auction trigger
     */
    virtual void endOfAuctionTrigger(int winnerId) override;

    virtual void distributePay(int auctionId, int auctionIteration, int winnerId, int payment, int wtpSum, int speed) override;
private:
    bool hasJoinEnded;
};

#endif /* APP_PLEXEAPPL_CONCRETE_MARKET_MARKETJOINERAGENT_H_ */
