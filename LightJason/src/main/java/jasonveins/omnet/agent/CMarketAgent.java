package jasonveins.omnet.agent;

import jasonveins.omnet.constants.CVariableBuilder;
import jasonveins.omnet.managers.AgentManager;
import org.lightjason.agentspeak.action.binding.IAgentAction;
import org.lightjason.agentspeak.action.binding.IAgentActionFilter;
import org.lightjason.agentspeak.action.binding.IAgentActionName;
import org.lightjason.agentspeak.common.CCommon;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;
import org.lightjason.agentspeak.generator.IBaseAgentGenerator;

import javax.annotation.Nonnull;
import java.io.InputStream;
import java.util.stream.Collectors;
import java.util.stream.Stream;


/**
 * Vehicle agent with market based capabilities.
 */
@IAgentAction
public class CMarketAgent extends IVehicleAgent<CMarketAgent> {

    //##########//
    //PROPERTIES//
    //##########//


    //###########//
    //CONSTRUCTOR//
    //###########//
    /**
     * ctor
     *
     * @param p_configuration agent configuration
     * @param m_am Reference to the agent manager responsible for this agent
     * @param m_id Identifier for this agent (must match omnet agent id)
     * @param m_vType The type of vehicle as specified in omnet .rou.xml file.
     */
    public CMarketAgent(@Nonnull IAgentConfiguration<CMarketAgent> p_configuration, @Nonnull AgentManager m_am, int m_id, @Nonnull String m_vType) {
        super(p_configuration, m_am, m_id, m_vType);
    }

    //#######################//
    //AGENT ACTION DEFINITION//
    //#######################//
    @IAgentActionFilter
    @IAgentActionName( name = "setup/auction")
    public void setupAuction(){
        System.out.println("Called setup Action");
    }

    @IAgentActionFilter
    @IAgentActionName( name = "send/resources")
    public void sendResources(){
        System.out.println("Called send resources");
    }

    @IAgentActionFilter
    @IAgentActionName( name = "send/bid")
    public void sendBid(){
        System.out.println("Called send bid");
    }

    @IAgentActionFilter
    @IAgentActionName( name = "store/bid")
    public void storeBid(){
        System.out.println("Called store bid");
    }

    //###############//
    //AGENT GENERATOR//
    //###############//
    public static class CMarketAgentGenerator extends IBaseAgentGenerator<CMarketAgent> {
        private AgentManager agentManager;
        /**
         * @param p_stream ASL code as any stream e.g. FileInputStream
         * @param p_am Reference to agent manager
         */
        public CMarketAgentGenerator(@Nonnull final InputStream p_stream, AgentManager p_am) throws Exception
        {
            super(
                    // input ASL stream
                    p_stream,
                    Stream.concat(
                            CCommon.actionsFromPackage(),
                            CCommon.actionsFromAgentClass( CMarketAgent.class )
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
        public final CMarketAgent generatesingle( final Object... p_data )
        {
            return new CMarketAgent( m_configuration, agentManager,(int)p_data[0],(String)p_data[1]);

        }
    }

}
