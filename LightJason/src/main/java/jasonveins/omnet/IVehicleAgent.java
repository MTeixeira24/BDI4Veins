package jasonveins.omnet;

import org.lightjason.agentspeak.agent.IBaseAgent;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;

import javax.annotation.Nonnull;

public abstract class IVehicleAgent extends IBaseAgent<IVehicleAgent> {

    private static final long serialVersionUID = -9102517014010783118L;
    protected int id;
    protected AgentManager agentManager;
    /**
     * ctor
     *
     * @param p_configuration agent configuration
     */
    public IVehicleAgent(@Nonnull IAgentConfiguration<IVehicleAgent> p_configuration, @Nonnull AgentManager m_am,@Nonnull final int m_id) {
        super(p_configuration);
        id = m_id;
        agentManager = m_am;
    }

    public int id(){return this.id;}
}
