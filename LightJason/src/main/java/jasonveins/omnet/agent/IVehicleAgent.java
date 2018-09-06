package jasonveins.omnet.agent;

import jasonveins.omnet.managers.AgentManager;
import jasonveins.omnet.managers.Constants;
import jasonveins.omnet.decision.InstructionModel;
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
    protected String vType;
    protected AgentManager agentManager;
    /**
     * ctor
     *
     * @param p_configuration agent configuration
     */
    public IVehicleAgent(@Nonnull IAgentConfiguration<T> p_configuration, @Nonnull AgentManager m_am,@Nonnull final int m_id, @Nonnull final String m_vType) {
        super(p_configuration);
        id = m_id;
        agentManager = m_am;
        vType = m_vType;
    }

    public int id(){return this.id;}

    public String vType(){return this.vType;}

    @IAgentActionFilter
    @IAgentActionName( name = "transmit/self/slow" )
    private void setControllerMaxSpeed()
    {
        InstructionModel iOb = new InstructionModel(this.id, 4);
        iOb.pushDouble(0.5);
        agentManager.addInstruction(iOb);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "transmit/other/slow" )
    private void sendSpeedDownMessage(@Nonnull final Number targetId)
    {
        InstructionModel iOb = new InstructionModel(this.id, Constants.REQUEST_SPEED_DOWN);
        iOb.pushInt(targetId.intValue());
        agentManager.addInstruction(iOb);
    }
}
