package jasonveins.omnet.agent;

import jasonveins.omnet.constants.CVariableBuilder;
import jasonveins.omnet.decision.InstructionModel;
import jasonveins.omnet.managers.AgentManager;
import jasonveins.omnet.managers.constants.VoteConstants;
import jasonveins.omnet.voting.CContext;
import org.lightjason.agentspeak.action.binding.IAgentAction;
import org.lightjason.agentspeak.action.binding.IAgentActionFilter;
import org.lightjason.agentspeak.action.binding.IAgentActionName;
import org.lightjason.agentspeak.common.CCommon;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;
import org.lightjason.agentspeak.generator.IBaseAgentGenerator;
import org.lightjason.agentspeak.language.CLiteral;
import org.lightjason.agentspeak.language.CRawTerm;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.CTrigger;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.ITrigger;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.Stream;

@IAgentAction
public class CRegroupVoter extends CVoterAgent {

    private static final long serialVersionUID = 8809996969656224479L;

    /**
     * ctor
     *
     * @param p_configuration       agent configuration
     * @param m_am                  Reference to the agent manager
     * @param m_id                  Id to be assigned to the new agent
     * @param m_vType               Vehicle type Id
     * @param voteRule              The single candidate vote rule this agent uses
     * @param m_factor              Factor used in utility functions
     * @param m_utility             Utility function identifier
     * @param m_committee_vote_rule The committee election voting rule this agent uses
     */
    public CRegroupVoter(@Nonnull IAgentConfiguration<CVoterAgent> p_configuration, @Nonnull AgentManager m_am, int m_id, @Nonnull String m_vType, String voteRule, double m_factor, String m_utility, String m_committee_vote_rule) {
        super(p_configuration, m_am, m_id, m_vType, voteRule, m_factor, m_utility, m_committee_vote_rule);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "open/vote" )
    protected void openNewVote(@Nonnull final String context, @Nonnull final List<Double> contextArgs){

        if(context.equals("regroup")){
            InstructionModel iOb = new InstructionModel(this.id, VoteConstants.NOTIFY_START_VOTE);

            ArrayList<Double> l_context = new ArrayList<>(), l_context_chair = new ArrayList<>();
            ArrayList<Integer> l_candidates = new ArrayList<>(contextArgs.size());

            int contextStart = (contextArgs.size() / 2);
            //First half contains the candidates
            contextArgs.stream().limit(contextStart).forEach(
                    dvalue -> l_candidates.add(dvalue.intValue())
            );
            //Second half their speeds
            contextArgs.stream().skip(contextStart).forEach(
                    l_context::add
            );
            //First parameter, what are the agents voting for?
            iOb.pushInt(VoteConstants.CONTEXT_REGROUP);
            //What information do the agents need to know about the candidates? Their preferred speed
            iOb.pushDoubleArray(l_context);
            //Save the context for later use
            m_context = new CContext(l_candidates, VoteConstants.CONTEXT_REGROUP, members.size());
            for(int i = 0 ; i < l_context.size(); i++){
                m_context.addContextArgument(l_candidates.get(i).toString(), l_context.get(i));
            }
            //Create a data structure for the leader. The leader can cast its vote
            l_context_chair.add((double)VoteConstants.CONTEXT_REGROUP);
            l_context_chair.addAll(l_context);
            //Finally specify the candidates
            iOb.pushIntArray(l_candidates);
            //Alert the controller of what size is expected a vote to be of.
            //Helps in message filtering.
            iOb.pushInt(committeeRule.getExpectedVoteSize(l_candidates.size()));
            //Add the instruction to the controller
            agentManager.addInstruction(iOb);
            final ITrigger l_trigger = CTrigger.from(
                    ITrigger.EType.ADDGOAL,
                    CLiteral.from(
                            "handle/vote/notification",
                            CRawTerm.from(l_candidates),
                            CRawTerm.from(l_context_chair)
                    )

            );
            this.trigger( l_trigger );

        }else{
            super.openNewVote(context, contextArgs);
        }
    }


    public static final class CRegroupVoterGenerator extends IBaseAgentGenerator<CVoterAgent> {

        private AgentManager agentManager;
        /**
         * @param p_stream ASL code as any stream e.g. FileInputStream
         * @param p_am Reference to agent manager
         */
        public CRegroupVoterGenerator(@Nonnull final InputStream p_stream, AgentManager p_am) throws Exception
        {
            super(
                    // input ASL stream
                    p_stream,
                    Stream.concat(
                            CCommon.actionsFromPackage(),
                            CCommon.actionsFromAgentClass( CRouterAgent.class )
                    ).collect(Collectors.toSet()),
                    new CVariableBuilder()
            );
            agentManager = p_am;
        }
        /**
         * generator method of the agent
         *
         * @param p_data any data which can be put from outside to the generator method
         * @return returns an agent
         */
        @Override
        public final CRegroupVoter generatesingle( @Nullable final Object... p_data )
        {
            assert p_data != null;
            return new CRegroupVoter( m_configuration, agentManager,(int)p_data[0], (String)p_data[1],
                    (String)p_data[2], (double)p_data[3], (String)p_data[4], (String)p_data[5]);
        }

    }
}
