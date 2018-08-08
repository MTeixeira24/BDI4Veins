package jasonveins.omnet;

import org.lightjason.agentspeak.configuration.IAgentConfiguration;

import javax.annotation.Nonnull;

public class NormalVehicleAgent extends IVehicleAgent {
    private static final long serialVersionUID = -5930719261251913002L;

    /**
     * ctor
     *
     * @param p_configuration agent configuration
     * @param m_am
     * @param m_id
     */
    public NormalVehicleAgent(@Nonnull IAgentConfiguration<IVehicleAgent> p_configuration, @Nonnull AgentManager m_am, @Nonnull int m_id) {
        super(p_configuration, m_am, m_id);
    }
}
