/*
 * InterferanceAppl.cpp
 *
 *  Created on: 18/10/2018
 *      Author: miguel
 */

#include "InterferanceAppl.h"

Define_Module(InterferanceAppl);

void InterferanceAppl::initialize(int stage){
    BaseApp::initialize(stage);
    if(stage == 1){
        myId = 100+myId;
    }
}
