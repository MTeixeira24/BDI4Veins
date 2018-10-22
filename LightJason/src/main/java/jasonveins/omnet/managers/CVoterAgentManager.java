package jasonveins.omnet.managers;

import jasonveins.omnet.agent.CVoterAgentGenerator;
import jasonveins.omnet.agent.IVehicleAgent;
import jasonveins.omnet.agent.NormalVehicleGenerator;
import jasonveins.omnet.environment.dijkstra.Graph;
import jasonveins.omnet.voting.Statistics.CJoinStatistics;

import javax.annotation.Nonnull;
import java.io.FileInputStream;

/**
 * Agent Manager to handle the creation of voting agents
 */
public class CVoterAgentManager extends AgentManager {

    private String voteRule;
    private double factor;
    private String utility;
    private String committee_vote_rule;
    private Graph scenarioRoute;

    /**
     * Class constructor
     *
     * @param m_aslpath Path to asl files
     * @param m_cm      Reference to the connection manager
     */
    public CVoterAgentManager(String m_aslpath, ConnectionManager m_cm) {
        super(m_aslpath, m_cm);
        voteRule = "Borda";
        stats = new CJoinStatistics();
        scenarioRoute = new Graph(resourceFolder + "json/routeJoin.json");
    }

    @Override
    public IVehicleAgent<?> buildAgent(int p_id, @Nonnull FileInputStream p_stream, @Nonnull String p_aslFile, @Nonnull String vType) {
        IVehicleAgent<?> l_ag = null;
        try {
            switch(vType){
                case "vVoter":
                    if(p_aslFile.equals(resourceFolder+"asl/voter.asl") || p_aslFile.equals(resourceFolder+"asl/iterativeVoter.asl")){
                        l_ag = new CVoterAgentGenerator(p_stream, this).generatesingle(p_id, vType, voteRule, factor, utility, committee_vote_rule);
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

    /**
     *
     * @param params platoonSize, iteration, rule, type, factor, utility (int, int, String, String, double, String)
     */
    @Override
    public void setSimParams(Object... params) {
        getStats().setSimParams((int)params[0], (int)params[1], (String)params[2], (String)params[3], (String)params[6]);
        voteRule = (String)params[2];
        committee_vote_rule = (String)params[6];
        factor = (double)params[4];
        utility = (String)params[5];
    }

    public CJoinStatistics getStats(){
        return (CJoinStatistics) stats;
    }

    public Graph getRoute() {
        return scenarioRoute;
    }
}
