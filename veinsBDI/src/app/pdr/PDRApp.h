/*
 * PDRApp.h
 *
 *  Created on: Jan 13, 2019
 *      Author: miguel
 */

#ifndef APP_PDR_PDRAPP_H_
#define APP_PDR_PDRAPP_H_

#include "veins/modules/application/platooning/apps/BaseApp.h"
#include "veins/modules/mobility/traci/TraCIConstants.h"
#include "../../messages/pdr/PDR_m.h"
class PDRApp : public BaseApp {
public:
    PDRApp();
    virtual ~PDRApp();
    virtual void initialize(int stage) override;
    virtual void finish() override;
    virtual void sendUnicast(cPacket* msg, int destination);
protected:
    virtual void handleLowerMsg(cMessage* msg) override;
    virtual void handleSelfMsg(cMessage* msg) override;
    void sendPDR();
private:
    const int pdrtype = 22345;
    cMessage* sendTimer;
    int sentPackets;
    int receivedPackets;
};

#endif /* APP_PDR_PDRAPP_H_ */
