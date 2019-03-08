package jasonveins.omnet.managers;

import jasonveins.omnet.agent.CBargainAgent;
import jasonveins.omnet.agent.IVehicleAgent;
import jasonveins.omnet.statistics.CMarketStatistics;

import javax.annotation.Nonnull;
import java.io.FileInputStream;
import java.nio.ByteBuffer;

public class CBargainAgentManager extends AgentManager {
    /**
     * Class constructor
     *
     * @param m_aslpath Path to asl files
     * @param m_cm      Reference to the connection manager
     */
    public CBargainAgentManager(String m_aslpath, ConnectionManager m_cm) {
        super(m_aslpath, m_cm);
    }

    /**
     * Build market agents based on their vType
     * @param p_id Id of the agent. Generated in OMNET and is equal to its corresponding control application
     * @param p_stream Input stream for the asl file
     * @param p_aslFile Name of the asl file. Must exist in folder: jasonveins.resources.asl
     * @param vType The vehicle type identifier as defined in SUMO
     * @return Reference to a market based agent
     */
    @Override
    public IVehicleAgent<?> buildAgent(int p_id, @Nonnull FileInputStream p_stream, @Nonnull String p_aslFile, @Nonnull String vType) {
        IVehicleAgent<?> l_ag = null;
        try {
            if ("vBargain".equals(vType)) {
                if (p_aslFile.equals(resourceFolder + "asl/bargainAgent.asl")) {
                    l_ag = new CBargainAgent.CBargainAgentGenerator(p_stream, this).generatesingle(p_id, vType);
                } else {
                    throw new RuntimeException("Invalid asl file specified for vehicle type " + vType + ". Got " + p_aslFile + "expected marketAgent.asl");
                }
            } else {
                throw new RuntimeException("Unknown vType for class: CBargainAgentManager");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return l_ag;
    }

    /**
     * Set the parameters for the current simulation, such as iteration number.
     * @param params The parameters for the agent manager
     */
    @Override
    public void setSimParams(ByteBuffer params) {

    }
}
