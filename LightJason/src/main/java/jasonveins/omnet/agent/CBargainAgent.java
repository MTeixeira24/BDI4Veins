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
import javax.annotation.Nullable;
import java.io.InputStream;
import java.util.stream.Collectors;
import java.util.stream.Stream;

@IAgentAction
public class CBargainAgent extends IVehicleAgent<CBargainAgent> {


    private int acceptThreshold;
    private int startingOffer;

    /**
     * ctor
     *
     * @param p_configuration agent configuration
     * @param m_am
     * @param m_id
     * @param m_vType
     */
    public CBargainAgent(@Nonnull IAgentConfiguration<CBargainAgent> p_configuration, @Nonnull AgentManager m_am, @Nonnull int m_id, @Nonnull String m_vType) {
        super(p_configuration, m_am, m_id, m_vType);
        acceptThreshold = 100;
        startingOffer = 20;
    }

    @IAgentActionFilter
    @IAgentActionName(name = "offer/send")
    public void sendOffer(){
        throw new RuntimeException("Unimplemented method, sendOffer");
    }

    @IAgentActionFilter
    @IAgentActionName(name = "offer/evaluate")
    public void evaluateOffer(){
        throw new RuntimeException("Unimplemented method, evaluateOffer");
    }


    public static class CBargainAgentGenerator extends IBaseAgentGenerator<CBargainAgent> {

        private AgentManager agentManager;
        /**
         * @param p_stream ASL code as any stream e.g. FileInputStream
         * @param p_am Reference to agent manager
         */
        public CBargainAgentGenerator(@Nonnull final InputStream p_stream, AgentManager p_am) throws Exception
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

        @Nullable
        @Override
        public CBargainAgent generatesingle(@Nullable Object... p_data) {
            assert p_data != null;
            return new CBargainAgent(m_configuration, agentManager,(int)p_data[0],(String)p_data[1]);
        }
    }
}
