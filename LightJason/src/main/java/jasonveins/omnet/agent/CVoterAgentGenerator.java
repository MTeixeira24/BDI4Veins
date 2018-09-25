package jasonveins.omnet.agent;

import jasonveins.omnet.constants.CVariableBuilder;
import jasonveins.omnet.managers.AgentManager;
import org.lightjason.agentspeak.common.CCommon;
import org.lightjason.agentspeak.generator.IBaseAgentGenerator;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.io.InputStream;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public final class CVoterAgentGenerator extends IBaseAgentGenerator<CVoterAgent> {

    private AgentManager agentManager;
    /**
     * @param p_stream ASL code as any stream e.g. FileInputStream
     * @param p_am Reference to agent manager
     */
    public CVoterAgentGenerator(@Nonnull final InputStream p_stream, AgentManager p_am) throws Exception
    {
        super(
                // input ASL stream
                p_stream,
                Stream.concat(
                        CCommon.actionsFromPackage(),
                        CCommon.actionsFromAgentClass( CVoterAgent.class )
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
    @Nullable
    @Override
    public final CVoterAgent generatesingle( @Nullable final Object... p_data )
    {
        assert p_data != null;
        final CVoterAgent agent = new CVoterAgent( m_configuration, agentManager,(int)p_data[0], (String)p_data[1], (int)p_data[2], (int)p_data[3], (String)p_data[4]);
        return agent;

    }

}