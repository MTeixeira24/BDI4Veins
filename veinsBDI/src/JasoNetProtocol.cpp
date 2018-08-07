/*
 * JasoNetProtocol.cpp
 *
 *  Created on: 07/08/2018
 *      Author: miguel
 */

#include "JasoNetProtocol.h"

JasoNetProtocol::JasoNetProtocol() {

}

JasoNetProtocol::~JasoNetProtocol(){

}

std::string JasoNetProtocol::buildSubscriptionRequest(uint32_t id){
    std::string msg ("Add-");
    msg.append(std::to_string(id));
    msg.append("\n");
    return msg;
}

std::string JasoNetProtocol::buildConnectionRequest(){
    std::string msg("Connect\n");
    return msg;
}

std::string JasoNetProtocol::buildBeliefUpdateRequest(uint32_t id, std::string belief, std::string value){
    std::string msg;
    if(belief.compare("speed") == 0){
            msg = "BeliefUpdate-";
            msg.append(std::to_string(id));
            msg.append("-speed-");
            msg.append(value);
            msg.append("\n");
    }
    return msg;
}

std::string JasoNetProtocol::parseResponse(std::string response, uint32_t *id, std::string & action){
    std::string data;
    if(response.compare("Ok\n") != 0){
        size_t pos = 0, spos = 0;
        std::string token, stoken;
        while ((pos = response.find(":")) != std::string::npos) {
            token = response.substr(0, pos);
            /*while ((spos = token.find("-")) != std::string::npos) {
                stoken = token.substr(0, spos);
                std::cout << stoken << std::endl;
                token.erase(0, spos + 1);
            }*/
            //TODO: Just to test commands from agents. Generalize into protocol
            spos = token.find("-");
            *id = std::stoi(token.substr(0, spos));
            token.erase(0, spos + 1);
            spos = token.find("-");
            action = token.substr(0, spos);
            token.erase(0, spos + 1);
            //spos = token.find("-");
            data = token;
            response.erase(0, pos + 1);
        }
    }else{
        action = "none";
    }
    return data;
}
