package jasonveins.omnet.agent;

import jasonveins.omnet.agent.utilityFunctions.CGaussianUtility;
import jasonveins.omnet.agent.utilityFunctions.IUtilityFunction;
import jasonveins.omnet.constants.CVariableBuilder;
import jasonveins.omnet.decision.InstructionModel;
import jasonveins.omnet.managers.AgentManager;
import jasonveins.omnet.managers.CMarketAgentManager;
import jasonveins.omnet.managers.constants.MarketConstants;
import jasonveins.omnet.market.CFirstPrice;
import jasonveins.omnet.market.CSecondPrice;
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
import java.util.List;
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

    private IAuctionModule auctionModule;
    private IUtilityFunction utilityFunction;
    private CMarketAgentManager marketManager;

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
                this.auctionModule = new CFirstPrice(this.id);
                break;
            }case "StochasticSearch":{
                this.auctionModule = new CStochasticLocalSearch(this.id);
                break;
            }case "SecondPrice":{
                this.auctionModule = new CSecondPrice(this.id);
                break;
            }
            default:{
                throw new RuntimeException("CMarketAgent: Unknown utility function type!");
            }
        }
        marketManager = (CMarketAgentManager)agentManager;
    }
    //###############//
    //UTILITY METHODS//
    //###############//

    private double amortizedUtilitySpeed(int pay, int result){
        double satisfaction = utilityFunction.computeUtilitySpeed(result);
        return amortizeUtility(pay, satisfaction);
    }

    private double amortizedUtilityRoute(int pay, List<Integer> result){
        double satisfaction = utilityFunction.computeUtilityRoute(result, ((CMarketAgentManager)agentManager).getRoute());
        return amortizeUtility(pay, satisfaction);
    }

    private double amortizeUtility(int pay, double utility){
        double dissatisfaction = 1 - utility;
        return utility + sfunction(pay)*dissatisfaction;
    }

    private double sfunction(int pay){
        double x = (double)pay/(double)auctionModule.getWtp();
        if(x > 1) x = 1;
        return x;
    }

    //#######################//
    //AGENT ACTION DEFINITION//
    //#######################//

    //#-------Setups----------#

    @IAgentActionFilter
    @IAgentActionName( name = "set/market/parameters")
    public void setMarketParameters(Number wtp, Number endowment, Number preferredSpeed, List<Integer> preferredRoute){
        auctionModule.setEndowment(endowment.intValue());
        auctionModule.setWtp(wtp.intValue());
        utilityFunction.setPreferredRoute(preferredRoute);
        utilityFunction.setPreferredSpeed(preferredSpeed.intValue());
        System.out.println("Received auction parameters: WTP:" + wtp.intValue() + " Money:" + endowment.intValue());
    }

    @IAgentActionFilter
    @IAgentActionName( name = "setup/auction")
    public void setupAuction(Number auctionContext, List<Integer> members){
        System.out.println("Called send resources for context " + auctionContext.intValue() +
                " with member list: " + members);
        if(auctionModule.hasEnded()){
            restartAuction();
        }
        InstructionModel iOb = auctionModule.setupAuction(auctionContext.intValue(), members);
        auctionModule.setManagerId(this.id);
        auctionModule.setContext(auctionContext.intValue());
        //Cast the managers bid
        if(auctionContext.intValue() != MarketConstants.CONTEXT_JOIN)
            auctionModule.addBid(this.id, auctionModule.castBid(MarketConstants.AUCTION_START));
        //Send the instruction over to the controller
        agentManager.addInstruction(iOb);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "setup/bidder")
    public void setupBidder(Number auctionId, Number context, Number managerId){
        System.out.println("Called to setup as a bidder for auctionId: " +
                auctionId.intValue() + " with context: " + context.intValue() +
                " for manager with id: " + managerId.intValue());
        if(auctionModule.hasEnded()){
            restartAuction();
        }
        if(context.intValue() == MarketConstants.CONTEXT_JOIN){
            marketManager.getStats().storeUtil(this.id, 0);
            marketManager.getStats().storeHammingUtil(this.id, 0);
            marketManager.getStats().storeHamming(this.id, 0);
        }
        auctionModule.setManagerId(managerId.intValue());
        auctionModule.setAuctionId(auctionId.intValue());
        auctionModule.setContext(context.intValue());
        agentManager.addInstruction(auctionModule.createBidInstruction(MarketConstants.AUCTION_START));
    }

    @IAgentActionFilter
    @IAgentActionName( name = "store/wtp")
    public void storeWtp(List<Integer> wtpList){
        auctionModule.setWtpSum(wtpList);
    }

    //#-------Bid process----------#

    @IAgentActionFilter
    @IAgentActionName( name = "send/bid")
    public void sendBid(Number auctionStatus){
        System.out.println("Called send bid");
        //Send a bid based on the previous result
        agentManager.addInstruction(auctionModule.createBidInstruction(auctionStatus.intValue()));
    }

    @IAgentActionFilter
    @IAgentActionName( name = "store/bid")
    public void storeBid(){
        System.out.println("Called store bid");
    }

    @IAgentActionFilter
    @IAgentActionName( name = "store/bid/list")
    public void storeBidList(List<Integer> idBidTuples){
        System.out.println("Storing a set of bids: " + idBidTuples);
        //Add all the bids
        for(int i = 0; i < idBidTuples.size(); i+=2)
            auctionModule.addBid(idBidTuples.get(i), idBidTuples.get(i+1));
        //This trigger is called with all possible bids, determine winner
        InstructionModel iOb = auctionModule.createWinnerInstruction();
        //Are all requirements met for the end of auction?
        if(auctionModule.endOfAuction()){
            iOb.setAgentAction(MarketConstants.HANDLE_END_OF_AUCTION);
            //If the manager is the one that jump straight to payment distribution
            iOb.pushInt(auctionModule.getDuePayment());
            if(this.id == auctionModule.getWinner()){
                if(auctionModule.getContext() == MarketConstants.CONTEXT_PATH){
                    marketManager.getStats().storeHammingUtil(this.id, 1.0);
                    marketManager.getStats().storeHamming(this.id, 9);
                }else if(auctionModule.getContext() == MarketConstants.CONTEXT_SPEED){
                    marketManager.getStats().storeUtil(this.id, 1.0);
                }
                iOb.pushInt(auctionModule.getWtpSum());
                iOb.pushInt(auctionModule.getContext());
                auctionModule.setEndowment(auctionModule.getEndowment() - auctionModule.getDuePayment());
            }
        }else{
            //Start a new iteration and add the managers bid
            auctionModule.startNewIteration();
            if(auctionModule.getContext() != MarketConstants.CONTEXT_JOIN){
                int auctionStatus = auctionModule.getWinner() == this.id ? MarketConstants.CONFIRMED : MarketConstants.REJECTED;
                auctionModule.addBid(this.id, auctionModule.castBid(auctionStatus ));
            }
        }
        agentManager.addInstruction(iOb);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "send/pay")
    public void sendPay(List<Integer> preferredPath, Number preferredSpeed, Number DuePay){
        InstructionModel iOb = auctionModule.createPayInstruction(DuePay.intValue());
        double util;
        switch (auctionModule.getContext()){
            case MarketConstants.CONTEXT_SPEED:{
                iOb.pushInt(preferredSpeed.intValue());
                util = amortizedUtilitySpeed(0, preferredSpeed.intValue());
                marketManager.getStats().storeUtil(this.id, util);
                break;
            }
            case MarketConstants.CONTEXT_PATH:{
                iOb.pushIntArray(preferredPath);
                util = amortizedUtilityRoute(0, preferredPath);
                marketManager.getStats().storeHammingUtil(this.id, util);
                marketManager.getStats().storeHamming(this.id, utilityFunction.getHammingDistance());
                break;
            }
            case MarketConstants.CONTEXT_JOIN:{
                iOb.pushInt(id);
                break;
            }
            default:
                throw new RuntimeException("sendPay: Unknown context");
        }
        agentManager.addInstruction(iOb);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "distribute/payment")
    public void distributePayment(Number payment, Object property, Number wtpWinner){
        InstructionModel iOb = new InstructionModel(this.id, MarketConstants.DISTRIBUTE_PAY);
        iOb.pushInt(auctionModule.getAuctionId());
        iOb.pushInt(auctionModule.getAuctionIteration());
        iOb.pushInt(auctionModule.getWinner());
        iOb.pushInt(payment.intValue());
        iOb.pushInt(auctionModule.getWtpSum() - wtpWinner.intValue());
        switch (auctionModule.getContext()){
            case MarketConstants.CONTEXT_SPEED:{
                Integer speed = (Integer)property;
                iOb.pushInt(speed);
                break;
            }
            case MarketConstants.CONTEXT_PATH:{
                List<Integer> route = (List<Integer>)property;
                iOb.pushIntArray(route);
                break;
            }
            case MarketConstants.CONTEXT_JOIN:{
                iOb.pushInt(auctionModule.getWinner());
                break;
            }
            default:
                throw new RuntimeException("sendPay: Unknown context");
        }
        int cut = (int)(payment.intValue() * ((double)auctionModule.getWtp() /
                (auctionModule.getWtpSum()-wtpWinner.intValue())));
        finalizeAuction( cut, property);
        agentManager.addInstruction(iOb);
    }

    @IAgentActionFilter
    @IAgentActionName(name = "finalize/auction")
    public void finalizeAuction(int payment, Object property){
        auctionModule.setEndowment(payment+auctionModule.getEndowment());
        double util = 0;
        if(id == 3){
            int stop = 0;
        }
        switch (auctionModule.getContext()){
            case MarketConstants.CONTEXT_SPEED:{
                Integer speed = (Integer)property;
                util = amortizedUtilitySpeed(payment, speed);
                marketManager.getStats().storeUtil(this.id, util);
                break;
            }
            case MarketConstants.CONTEXT_PATH:{
                List<Integer> route = (List<Integer>)property;
                util = amortizedUtilityRoute(payment, route);
                marketManager.getStats().storeHammingUtil(this.id, util);
                marketManager.getStats().storeHamming(this.id, utilityFunction.getHammingDistance());
                break;
            }
            case MarketConstants.CONTEXT_JOIN:{
                break;
            }
            default:
                throw new RuntimeException("sendPay: Unknown context");
        }
        System.out.println("Total utility: " + util);
        //TODO: Update statistics
    }

    private void restartAuction(){
        CFirstPrice newAuction = new CFirstPrice(this.id);

        newAuction.setEndowment(auctionModule.getEndowment());
        newAuction.setWtp(auctionModule.getWtp());
        newAuction.setWtpSum(auctionModule.getWtpSum());

        auctionModule = newAuction;
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
