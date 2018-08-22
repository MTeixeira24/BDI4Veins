package jasonveins.omnet;

import org.lightjason.agentspeak.action.binding.IAgentAction;
import org.lightjason.agentspeak.action.binding.IAgentActionFilter;
import org.lightjason.agentspeak.action.binding.IAgentActionName;
import org.lightjason.agentspeak.agent.IAgent;
import org.lightjason.agentspeak.agent.IBaseAgent;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;

import javax.annotation.Nonnull;


@IAgentAction
public abstract class IVehicleAgent<T extends IAgent<?>> extends IBaseAgent<T> {

    private static final long serialVersionUID = -9102517014010783118L;
    protected int id;
    protected AgentManager agentManager;
    /**
     * ctor
     *
     * @param p_configuration agent configuration
     */
    public IVehicleAgent(@Nonnull IAgentConfiguration<T> p_configuration, @Nonnull AgentManager m_am,@Nonnull final int m_id) {
        super(p_configuration);
        id = m_id;
        agentManager = m_am;
    }

    public int id(){return this.id;}

    @IAgentActionFilter
    @IAgentActionName( name = "transmit/self/slow" )
    private void transmitDecision()
    {
        String msg = this.id() + "-setMaxSpeed-0.5-";
        agentManager.addInstruction(msg);
    }
}
