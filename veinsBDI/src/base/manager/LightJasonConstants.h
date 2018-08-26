/*
 * LightJasonConstants.h
 *
 *  Created on: 26/08/2018
 *      Author: miguel
 */

#ifndef BASE_MANAGER_LIGHTJASONCONSTANTS_H_
#define BASE_MANAGER_LIGHTJASONCONSTANTS_H_

/*
 * Request type codes
 */

#define CONNECTION_REQUEST 0x01

#define ADD_AGENT 0x02

#define REMOVE_AGENT 0x03

#define SET_MAX_SPEED 0x04

#define QUERY 0x05

#define TERMINATE_CONNECTION 0x7E

/*
 * Constants to determine value type in message
 */


#define VALUE_BOOL 0x01

#define VALUE_CHAR 0x02

#define VALUE_SHORT 0x03

#define VALUE_INT 0x04

#define VALUE_LONG 0x05

#define VALUE_FLOAT 0x06

#define VALUE_DOUBLE 0x07


#endif /* BASE_MANAGER_LIGHTJASONCONSTANTS_H_ */