package jasonveins.omnet.agent.voting;

import jasonveins.omnet.agent.CBargainAgent;
import jasonveins.omnet.agent.IVehicleAgent;
import jasonveins.omnet.agent.utilityFunctions.CGaussianUtility;
import jasonveins.omnet.agent.utilityFunctions.IUtilityFunction;
import jasonveins.omnet.constants.CVariableBuilder;
import jasonveins.omnet.environment.dijkstra.Graph;
import jasonveins.omnet.environment.dijkstra.Vertex;
import jasonveins.omnet.managers.AgentManager;
import jasonveins.omnet.managers.CAgentCreationQueue;
import jasonveins.omnet.voting.CContext;
import jasonveins.omnet.voting.rule.CApproval;
import jasonveins.omnet.voting.rule.IRule;
import org.lightjason.agentspeak.action.binding.IAgentAction;
import org.lightjason.agentspeak.action.binding.IAgentActionFilter;
import org.lightjason.agentspeak.action.binding.IAgentActionName;
import org.lightjason.agentspeak.common.CCommon;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;
import org.lightjason.agentspeak.generator.IBaseAgentGenerator;
import org.lightjason.agentspeak.language.instantiable.plan.IPlan;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;
import java.util.stream.Stream;

@IAgentAction
public class CVoterAgent extends IVehicleAgent<CVoterAgent> {

    protected CContext m_context;
    protected IUtilityFunction utility;
    protected CVoteParameters votingState;
    protected CommitteeRule committeeRule;
    protected SingleRule singleRule;

    /**
     * ctor
     *
     * @param p_configuration agent configuration
     * @param m_am
     * @param m_id
     * @param m_vType
     */
    public CVoterAgent(@Nonnull IAgentConfiguration<CVoterAgent> p_configuration,
                       @Nonnull AgentManager m_am, @Nonnull int m_id, @Nonnull String m_vType) {
        super(p_configuration, m_am, m_id, m_vType);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "initialize/agent" )
    private void initializeAgent(Number p_isLeader, String p_utilityFunct, Number p_utilityFactor,
                                 String p_singleRule, String p_comRule, List<Integer> p_routeMap,
                                 List<Integer> p_members){

    }


    public static class CVoterAgentGenerator extends IBaseAgentGenerator<CVoterAgent> {
        private AgentManager agentManager;
        private CAgentCreationQueue insertionQueue;
        /**
         * @param p_stream ASL code as any stream e.g. FileInputStream
         * @param p_am Reference to agent manager
         */
        public CVoterAgentGenerator(@Nonnull final InputStream p_stream, AgentManager p_am) throws Exception
        {
            this(p_stream, p_am, null);
        }

        public CVoterAgentGenerator(@Nonnull final InputStream p_stream, AgentManager p_am,
                                    CAgentCreationQueue p_insertionQueue) throws Exception
        {
            super(
                    // input ASL stream
                    p_stream,
                    Stream.concat(
                            CCommon.actionsFromPackage(),
                            CCommon.actionsFromAgentClass( CBargainAgent.class )
                    ).collect(Collectors.toSet()),
                    new CVariableBuilder()
            );
            agentManager = p_am;
            insertionQueue = p_insertionQueue;
        }

        @Nullable
        @Override
        public CVoterAgent generatesingle(@Nullable Object... p_data) {
            assert p_data != null;
            if(insertionQueue != null){
                CAgentCreationQueue.CAgentData agentData = insertionQueue.popAgent();
                final CVoterAgent agent = new CVoterAgent(m_configuration, agentManager, agentData.getId(), agentData.getvType());
                agentManager.addAgentToMap(agent);
                return agent;
            }else
                return new CVoterAgent(m_configuration, agentManager,(int)p_data[0],(String)p_data[1]);
        }

        public Set<IPlan> getPlans(){
            return m_configuration.plans();
        }
    }
}
