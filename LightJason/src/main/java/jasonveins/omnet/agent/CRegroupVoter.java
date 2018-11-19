package jasonveins.omnet.agent;

import jasonveins.omnet.constants.CVariableBuilder;
import jasonveins.omnet.managers.AgentManager;
import org.lightjason.agentspeak.common.CCommon;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;
import org.lightjason.agentspeak.generator.IBaseAgentGenerator;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.io.InputStream;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public class CRegroupVoter extends CVoterAgent {
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
