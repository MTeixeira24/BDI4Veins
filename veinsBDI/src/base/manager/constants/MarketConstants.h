/*
 * MarketConstants.h
 *
 *  Created on: Dec 28, 2018
 *      Author: miguel
 */

#ifndef BASE_MANAGER_CONSTANTS_MARKETCONSTANTS_H_
#define BASE_MANAGER_CONSTANTS_MARKETCONSTANTS_H_

#include "../LightJasonConstants.h"

#define SUBMIT_BID 0x09

#define SEND_AUCTION_RESULTS 0x0A

#define NOTIFY_START_AUCTION 0x0C

#define HANDLE_END_OF_AUCTION 0x0D

#define SEND_PAY 0x0E

#define DISTRIBUTE_PAY 0x0F

#define CONTEXT_SPEED 0x00

#define CONTEXT_JOIN 0x01

#define CONTEXT_PATH 0x02

#define CONTEXT_REGROUP 0x03

#define AUCTION_START 0x00

#define REJECTED 0x01

#define CONFIRMED 0X02

#define DECOMMITTED 0x03
#endif /* BASE_MANAGER_CONSTANTS_MARKETCONSTANTS_H_ */
