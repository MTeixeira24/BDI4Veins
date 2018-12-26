package jasonveins.omnet.agent;

import jasonveins.omnet.agent.utilityFunctions.CGaussianUtility;
import jasonveins.omnet.agent.utilityFunctions.IUtilityFunction;
import jasonveins.omnet.constants.CVariableBuilder;
import jasonveins.omnet.managers.AgentManager;
import jasonveins.omnet.market.CFirstPrice;
import jasonveins.omnet.market.CStochasticLocalSearch;
import jasonveins.omnet.market.IAuctionModule;
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

    IAuctionModule auctionModule;
    IUtilityFunction utilityFunction;

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
    public CMarketAgent(@Nonnull IAgentConfiguration<CMarketAgent> p_configuration, @Nonnull AgentManager m_am,
                        int m_id, @Nonnull String m_vType, String utilityFunction, String auctionModule) {
        super(p_configuration, m_am, m_id, m_vType);
        switch (utilityFunction){
            case "distribution":{
                throw new RuntimeException("CMarketAgent: Unknown utility function type!");
            }case "khan":{
                throw new RuntimeException("CMarketAgent: Unknown utility function type!");
            }case "gaussian":{
                this.utilityFunction = new CGaussianUtility();
                break;
            }default:{
                throw new RuntimeException("CMarketAgent: Unknown utility function type!");
            }
        }

        switch (auctionModule){
            case "FirstPrice":{
                this.auctionModule = new CFirstPrice();
                break;
            }case "StochasticSearch":{
                this.auctionModule = new CStochasticLocalSearch();
                break;
            }default:{
                throw new RuntimeException("CMarketAgent: Unknown utility function type!");
            }
        }
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
    @IAgentActionName( name = "set/market/parameters")
    public void setMarketParameters(Number wtp, Number endowment){
        auctionModule.setEndowment(endowment.intValue());
        auctionModule.setWtp(wtp.intValue());
        System.out.println("Received auction parameters: WTP:" + wtp.intValue() + " Money:" + endowment.intValue());
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
            //Inputs: utility function and auction module
            return new CMarketAgent( m_configuration, agentManager,(int)p_data[0],(String)p_data[1],
                    (String)p_data[2], (String)p_data[3]);
        }
    }

}
