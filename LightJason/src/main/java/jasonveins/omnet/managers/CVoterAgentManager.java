package jasonveins.omnet.managers;

import jasonveins.omnet.agent.CVoterAgent;
import jasonveins.omnet.agent.CVoterAgentGenerator;
import jasonveins.omnet.agent.IVehicleAgent;
import jasonveins.omnet.agent.NormalVehicleGenerator;

import javax.annotation.Nonnull;
import java.io.FileInputStream;

/**
 * Agent Manager to handle the creation of voting agents
 */
public class CVoterAgentManager extends AgentManager {
    /**
     * Class constructor
     *
     * @param m_aslpath Path to asl files
     * @param m_cm      Reference to the connection manager
     */
    public CVoterAgentManager(String m_aslpath, ConnectionManager m_cm) {
        super(m_aslpath, m_cm);
    }

    @Override
    public IVehicleAgent<?> buildAgent(int p_id, @Nonnull FileInputStream p_stream, @Nonnull String p_aslFile, @Nonnull String vType) {
        IVehicleAgent<?> l_ag = null;
        try {
            switch(vType){
                case "vVoter":
                    if(p_aslFile.equals(resourceFolder+"fuelVoter.asl") || p_aslFile.equals(resourceFolder+"speedVoter.asl") || p_aslFile.equals(resourceFolder+"voter.asl")){
                        l_ag = new CVoterAgentGenerator(p_stream, this).generatesingle(p_id, vType);
                    }else{
                        throw new RuntimeException("Invalid asl file specified for vehicle type " + vType +". Got " + p_aslFile + "expected FuelVoter.asl, SpeedVoter.asl or voter.asl");
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
}
