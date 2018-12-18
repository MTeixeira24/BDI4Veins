package jasonveins.omnet.agent;

import jasonveins.omnet.managers.AgentManager;
import org.lightjason.agentspeak.action.binding.IAgentAction;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;

import javax.annotation.Nonnull;


/**
 * Vehicle agent with market based capabilities.
 */
@IAgentAction
public class CMarketAgent extends IVehicleAgent<CMarketAgent> {
    /**
     * ctor
     *
     * @param p_configuration agent configuration
     * @param m_am Reference to the agent manager responsible for this agent
     * @param m_id Identifier for this agent (must match omnet agent id)
     * @param m_vType The type of vehicle as specified in omnet .rou.xml file.
     */
    public CMarketAgent(@Nonnull IAgentConfiguration<CMarketAgent> p_configuration, @Nonnull AgentManager m_am, @Nonnull int m_id, @Nonnull String m_vType) {
        super(p_configuration, m_am, m_id, m_vType);
    }


}
