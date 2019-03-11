package jasonveins.omnet.agent;

import junit.framework.TestCase;

import java.io.FileInputStream;

public class CBargainAgentTest extends TestCase {

    private static final String resourceFolder = "src/main/resources/";

    public void testCreation(){
        IVehicleAgent<?> l_ag = null;
        int p_id = 0;
        String vType = "vBargain";
        String aslFile = resourceFolder + "asl/bargainAgent.asl";
        try
                (
                        final FileInputStream l_stream = new FileInputStream( aslFile )
                ){
            l_ag = new CBargainAgent.CBargainAgentGenerator(l_stream, null).generatesingle(p_id, vType);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void testSendOffer() {
    }

    public void testEvaluateOffer() {
    }

    public void testReceiveOfferPayout() {
    }
}