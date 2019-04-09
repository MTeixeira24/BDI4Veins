package jasonveins.omnet.managers;

import jasonveins.omnet.agent.CVoterAgentGenerator;
import jasonveins.omnet.agent.IVehicleAgent;
import jasonveins.omnet.agent.NormalVehicleGenerator;
import jasonveins.omnet.agent.voting.CVoterAgent;
import jasonveins.omnet.environment.dijkstra.Graph;
import jasonveins.omnet.statistics.CBaseStatistics;
import jasonveins.omnet.statistics.CJoinStatistics;

import javax.annotation.Nonnull;
import java.io.FileInputStream;
import java.nio.ByteBuffer;
import java.util.Set;
import java.util.stream.Collectors;

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

    @Override
    public Set<IVehicleAgent<?>> buildAgentBulk(@Nonnull FileInputStream p_stream, @Nonnull  CAgentCreationQueue p_queue){
        Set<IVehicleAgent<?> > newAgents = null;
        try{
            CVoterAgent.CVoterAgentGenerator gen = new CVoterAgent.CVoterAgentGenerator(
                    p_stream, this, p_queue, voteRule, utility, committee_vote_rule);
            newAgents = gen.generatemultiple(p_queue.size()).collect(Collectors.toSet());
            setupPlanMap(gen.getPlans());
        }catch (Exception e){
            e.printStackTrace();
            System.exit(2);
        }
        return newAgents;
    }

    /**
     *
     * @param params The parameters for the agent manager
     */
    @Override
    public void setSimParams(ByteBuffer params) {
        String statsType = CByteUtils.extractString(params);
        switch (statsType){
            case "CJoinStatistics":{
                stats = new CJoinStatistics();
                break;
            }
            case "CBaseStatistics":{
                stats = new CBaseStatistics();
                break;
            }
            default:
                throw new RuntimeException("VoterAgentManager: Unknown stats type");
        }
        int platoonSize = params.getInt();
        String rule = CByteUtils.extractString(params);
        String type = CByteUtils.extractString(params);
        int iteration = params.getInt();
        double factor = params.getDouble();
        String utility = CByteUtils.extractString(params);
        String committee_vote_rule = CByteUtils.extractString(params);
        getStats().setSimParams(platoonSize,iteration,rule,type,committee_vote_rule);

        //Create a new file output stream.
        /*PrintStream fileOut = null;
        PrintStream fileErr = null;
        try {
            fileOut = new PrintStream("logs/out"+iteration+".txt");
            fileErr = new PrintStream("logs/err"+iteration+".txt");
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        System.setOut(fileOut);
        System.setErr(fileErr);*/
        voteRule = rule;
        this.committee_vote_rule = committee_vote_rule;
        this.factor = factor;
        this.utility = utility;
        /*getStats().setSimParams((int)params[0], (int)params[1], (String)params[2], (String)params[3], (String)params[6]);
        voteRule = (String)params[2];
        committee_vote_rule = (String)params[6];
        factor = (double)params[4];
        utility = (String)params[5];*/
    }

    public CJoinStatistics getStats(){
        return (CJoinStatistics) stats;
    }

    public Graph getRoute() {
        return scenarioRoute;
    }
}
