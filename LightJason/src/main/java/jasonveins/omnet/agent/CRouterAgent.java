package jasonveins.omnet.agent;

import jasonveins.omnet.constants.CVariableBuilder;
import jasonveins.omnet.decision.InstructionModel;
import jasonveins.omnet.managers.AgentManager;
import org.lightjason.agentspeak.action.binding.IAgentAction;
import org.lightjason.agentspeak.action.binding.IAgentActionFilter;
import org.lightjason.agentspeak.action.binding.IAgentActionName;
import org.lightjason.agentspeak.common.CCommon;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;
import org.lightjason.agentspeak.generator.IBaseAgentGenerator;
import org.lightjason.agentspeak.language.CLiteral;
import org.lightjason.agentspeak.language.CRawTerm;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.Stream;


@IAgentAction
public final class CRouterAgent extends IVehicleAgent<CRouterAgent> {



    private static final long serialVersionUID = -563558245660729346L;

    private final int SEND_ROUTE = 9;

    /**
     * ctor
     *
     * @param p_configuration agent configuration
     * @param m_am Reference to the agent manager
     * @param m_id Id to be assigned to this agent. Should match the OMNeT node
     * @param m_vType The vehicle type specified in the OMNeT route file.
     */
    public CRouterAgent(@Nonnull IAgentConfiguration<CRouterAgent> p_configuration, @Nonnull AgentManager m_am, @Nonnull int m_id, @Nonnull String m_vType) {
        super(p_configuration, m_am, m_id, m_vType);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "send/route")
    private void sendRoute(List<String> node){
        System.out.println("Got a request to send down the route: " + node.toString());
        ArrayList<Integer> integerList = new ArrayList<>(node.size());
        node.forEach(
                n -> integerList.add(Integer.parseInt(n.split("n")[1]))
        );
        System.out.println("Parsed list: " + integerList.toString());
        InstructionModel iOb = new InstructionModel(this.id, SEND_ROUTE);
        iOb.pushIntArray(integerList);
        agentManager.addInstruction(iOb);
    }

    public static final class CRouterAgentGenerator extends IBaseAgentGenerator<CRouterAgent> {

        private AgentManager agentManager;
        /**
         * @param p_stream ASL code as any stream e.g. FileInputStream
         * @param p_am Reference to agent manager
         */
        public CRouterAgentGenerator(@Nonnull final InputStream p_stream, AgentManager p_am) throws Exception
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
        public final CRouterAgent generatesingle( @Nullable final Object... p_data )
        {
            assert p_data != null;
            return new CRouterAgent( m_configuration, agentManager,(int)p_data[0], (String)p_data[1]);
        }

    }

}
