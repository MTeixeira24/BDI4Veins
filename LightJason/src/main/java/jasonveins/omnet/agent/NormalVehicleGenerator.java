package jasonveins.omnet.agent;

import jasonveins.omnet.managers.AgentManager;
import jasonveins.omnet.constants.CVariableBuilder;
import jasonveins.omnet.managers.CAgentCreationQueue;
import org.lightjason.agentspeak.common.CCommon;
import org.lightjason.agentspeak.generator.IBaseAgentGenerator;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.io.InputStream;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public final class NormalVehicleGenerator extends IBaseAgentGenerator<NormalVehicleAgent> {

    private AgentManager agentManager;

    private CAgentCreationQueue insertionQueue;
    /**
     * @param p_stream ASL code as any stream e.g. FileInputStream
     */
    public NormalVehicleGenerator(@Nonnull final InputStream p_stream, AgentManager m_am) throws Exception
    {
        this(p_stream, m_am, null);
    }

    /**
     * @param p_stream ASL code as any stream e.g. FileInputStream
     */
    public NormalVehicleGenerator(@Nonnull final InputStream p_stream, AgentManager m_am, CAgentCreationQueue p_insertionQueue) throws Exception
    {
        super(
                // input ASL stream
                p_stream,
                Stream.concat(
                        CCommon.actionsFromPackage(),
                        CCommon.actionsFromAgentClass( NormalVehicleAgent.class )
                ).collect(Collectors.toSet()),
                new CVariableBuilder()
        );
        agentManager = m_am;
        insertionQueue = p_insertionQueue;
    }
    /**
     * generator method of the agent
     *
     * @param p_data any data which can be put from outside to the generator method
     * @return returns an agent
     */
    @Nullable
    @Override
    public final NormalVehicleAgent generatesingle( @Nullable final Object... p_data )
    {
        assert p_data != null;
        if(insertionQueue != null){
            CAgentCreationQueue.CAgentData agentData = insertionQueue.popAgent();
            final NormalVehicleAgent agent = new NormalVehicleAgent(m_configuration, agentManager, agentData.getId(), agentData.getvType());
            agentManager.addAgentToMap(agent);
            return agent;
        }else{
            if(p_data.length == 1){
                final NormalVehicleAgent agent = new NormalVehicleAgent( m_configuration, agentManager,(int)p_data[0]);
                return agent;
            }else{
                final NormalVehicleAgent agent = new NormalVehicleAgent( m_configuration, agentManager,(int)p_data[0], (String)p_data[1]);
                return agent;
            }
        }

    }
}
