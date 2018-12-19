package jasonveins.omnet.managers;

import jasonveins.omnet.agent.CRouterAgent;
import jasonveins.omnet.agent.IVehicleAgent;
import jasonveins.omnet.agent.NormalVehicleGenerator;
import jasonveins.omnet.statistics.CBaseStatistics;

import javax.annotation.Nonnull;
import java.io.FileInputStream;
import java.nio.ByteBuffer;

public class CRouterManager extends AgentManager {
    /**
     * Class constructor
     *
     * @param m_aslpath Path to asl files
     * @param m_cm      Reference to the connection manager
     */
    public  CRouterManager(String m_aslpath, ConnectionManager m_cm) {
        super(m_aslpath, m_cm);
    }

    @Override
    public IVehicleAgent<?> buildAgent(int p_id, @Nonnull FileInputStream p_stream, @Nonnull String p_aslFile, @Nonnull String vType) {
        IVehicleAgent<?> l_ag = null;
        try {
            switch(vType){
                case "vRouter":
                    if(p_aslFile.equals(resourceFolder+"asl/router.asl")){
                        l_ag = new CRouterAgent.CRouterAgentGenerator(p_stream, this).generatesingle(p_id, vType);
                    }else{
                        throw new RuntimeException("Invalid asl file specified for vehicle type " + vType +". Got " + p_aslFile + "expected router.asl");
                    }
                    break;
                default:
                    l_ag = new NormalVehicleGenerator(p_stream, this).generatesingle(p_id, vType);
                    break;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return l_ag;
    }

    /**
     *
     * @param params The parameters for the agent manager
     */
    @Override
    public void setSimParams(ByteBuffer params) {
        //String statsType = CByteUtils.extractString(params);
        int iteration = params.getInt();
        stats = new CBaseStatistics();
        stats.setSimParams(iteration);
    }
}
