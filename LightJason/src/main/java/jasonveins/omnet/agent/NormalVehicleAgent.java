package jasonveins.omnet.agent;

import jasonveins.omnet.managers.AgentManager;
import org.lightjason.agentspeak.action.binding.IAgentAction;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;

import javax.annotation.Nonnull;

@IAgentAction
public class NormalVehicleAgent extends IVehicleAgent<NormalVehicleAgent> {
    private static final long serialVersionUID = -5930719261251913002L;

    /**
     * ctor
     *
     * @param p_configuration agent configuration
     * @param m_am
     * @param m_id
     */
    public NormalVehicleAgent(@Nonnull IAgentConfiguration<NormalVehicleAgent> p_configuration, @Nonnull AgentManager m_am, @Nonnull int m_id) {
        this(p_configuration, m_am, m_id, "none");
    }
    public NormalVehicleAgent(@Nonnull IAgentConfiguration<NormalVehicleAgent> p_configuration, @Nonnull AgentManager m_am, @Nonnull int m_id, @Nonnull String vType) {
        super(p_configuration, m_am, m_id, vType);
    }



}
