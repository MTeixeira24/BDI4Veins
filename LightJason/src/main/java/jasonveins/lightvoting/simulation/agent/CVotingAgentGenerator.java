package jasonveins.lightvoting.simulation.agent;

import org.lightjason.agentspeak.common.CCommon;
import org.lightjason.agentspeak.generator.IBaseAgentGenerator;
import jasonveins.lightvoting.simulation.action.message.CSend;
import jasonveins.lightvoting.simulation.constants.CVariableBuilder;
import jasonveins.lightvoting.simulation.environment.CEnvironment;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.io.InputStream;
import java.text.MessageFormat;
import java.util.concurrent.atomic.AtomicLong;
import java.util.stream.Collectors;
import java.util.stream.Stream;

/**
 * Class CVotingAgentGenerator
 */
public final class CVotingAgentGenerator extends IBaseAgentGenerator<CVotingAgent>
{

    /**
     * Store reference to send action to registered agents upon creation.
     */
    private final CSend m_send;

    /**
     * Current free agent id, needs to be thread-safe, therefore using AtomicLong.
     */
    private final AtomicLong m_agentcounter = new AtomicLong();

    /**
     * environment reference
     */
    private final CEnvironment m_environment;

    /**
     * number of alternatives
     */
    private final int m_altNum;
    private final String m_grouping;
    private final String m_fileName;

    /**
     * constructor of the generator
     * @param p_stream ASL code as any stream e.g. FileInputStream
     * @param p_altNum number of alternatives
     * @param p_grouping grouping algorithm
     * @param p_fileName h5 file
     * @throws Exception Thrown if something goes wrong while generating agents.
     */
    public CVotingAgentGenerator(final CSend p_send, @Nonnull final InputStream p_stream, final CEnvironment p_environment, final int p_altNum,
                                 final String p_grouping,
                                 final String p_fileName
    ) throws Exception
    {

        super(
                // input ASL stream
                p_stream,

                // a set with all possible actions for the agent
                Stream.concat(
                        // we use all build-in actions of LightJason
                        CCommon.actionsFromPackage(),
                        Stream.concat(
                                // use the actions which are defined inside the agent class
                                CCommon.actionsFromAgentClass( CVotingAgent.class ),
                                // add VotingAgent related external actions
                                Stream.of(
                                        p_send
                                )
                        )
                        // build the set with a collector
                ).collect( Collectors.toSet() ),/*

                // aggregation function for the optimization function, here
                // we use an empty function
                IAggregation.EMPTY,*/

                // variable builder
                new CVariableBuilder( p_environment )
        );

        m_send = p_send;
        m_environment = p_environment;
        m_altNum = p_altNum;
        m_grouping = p_grouping;
        m_fileName = p_fileName;
    }

    // unregister an agent
    // @param p_agent agent object
    public final void unregister( final CVotingAgent p_agent )
    {
        m_send.unregister( p_agent );
    }

    // generator method of the agent
    // @param p_data any data which can be put from outside to the generator method
    // @return returns an agent
    @Nullable
    @Override
    public final CVotingAgent generatesingle( final Object... p_data )
    {
        // register a new agent object at the send action and the register
        // method retruns the object reference

        final CVotingAgent l_votingAgent = new CVotingAgent(

                // create a string with the agent name "agent <number>"
                // get the value of the counter first and increment, build the agent
                // name with message format (see Java documentation)
                MessageFormat.format( "agent {0}", m_agentcounter.getAndIncrement() ),

                // add the agent configuration
                m_configuration,
                // add the chair agent
                ( (CChairAgent.CChairAgentGenerator) p_data[0] ).generatesingle(),
                m_environment,
                m_altNum,
                m_grouping,
                m_fileName
        );

        l_votingAgent.sleep( Integer.MAX_VALUE  );
        m_environment.initialset( l_votingAgent );
        return m_send.register( l_votingAgent );

    }
}
