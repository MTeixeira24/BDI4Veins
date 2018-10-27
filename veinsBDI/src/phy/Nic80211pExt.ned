package veinsbdi.phy;

import veinsbdi.phy.PhyLayer80211pExt;
import org.car2x.veins.modules.mac.ieee80211p.Mac1609_4;
import org.car2x.veins.modules.nic.INic80211p;

module Nic80211pExt like INic80211p
{
    gates:
        input upperLayerIn; // to upper layers
        output upperLayerOut; // from upper layers
        output upperControlOut; // control information
        input upperControlIn; // control information
        input radioIn; // radioIn gate for sendDirect

    submodules:
        phy80211p: PhyLayer80211pExt {
            @display("p=69,218;i=block/process_s");
        }

        mac1609_4: Mac1609_4 {
            @display("p=69,82");
        }

    connections:
        radioIn --> phy80211p.radioIn;

        mac1609_4.lowerControlOut --> phy80211p.upperControlIn;
        mac1609_4.lowerLayerOut --> phy80211p.upperLayerIn;
        phy80211p.upperLayerOut --> mac1609_4.lowerLayerIn;
        phy80211p.upperControlOut --> mac1609_4.lowerControlIn;
        
        mac1609_4.upperControlIn <-- upperControlIn;
        mac1609_4.upperLayerIn <-- upperLayerIn;

        mac1609_4.upperLayerOut --> upperLayerOut;
        mac1609_4.upperControlOut --> upperControlOut;
}
