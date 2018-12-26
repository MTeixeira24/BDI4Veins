package jasonveins;

import jasonveins.omnet.agent.CMarketAgent;
import jasonveins.omnet.agent.CRegroupVoter;
import jasonveins.omnet.agent.CRouterAgent;
import jasonveins.omnet.agent.CVoterAgentGenerator;
import jasonveins.omnet.agent.utilityFunctions.CGaussianUtility;
import jasonveins.omnet.managers.CMarketAgentManager;
import jasonveins.omnet.managers.CRouterManager;
import jasonveins.omnet.managers.constants.MarketConstants;
import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;
import org.apache.commons.lang.math.IntRange;
import org.lightjason.agentspeak.agent.IAgent;
import org.lightjason.agentspeak.language.CLiteral;
import org.lightjason.agentspeak.language.CRawTerm;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.CTrigger;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.ITrigger;

import java.io.FileInputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Set;
import java.util.stream.Collectors;
import java.util.stream.DoubleStream;
import java.util.stream.IntStream;

/**
 * Unit test for simple App.
 */
public class AppTest 
    extends TestCase
{
    /**
     * Create the test case
     *
     * @param testName name of the test case
     */
    public AppTest( String testName )
    {
        super( testName );
    }

    /**
     * @return the suite of tests being tested
     */
    public static Test suite()
    {
        return new TestSuite( AppTest.class );
    }

    /**
     * Rigourous Test :-)
     */
    public void testApp() {
    }

    public void testSpeedUtil(){
        CGaussianUtility util = new CGaussianUtility(300, 7);
        util.setPreferredSpeed(100);
        double value1 = util.computeUtilitySpeed(90);
        double value2 = util.computeUtilitySpeed(85);
        double value3 = util.computeUtilitySpeed(120);
        assertTrue(value1 > 0.71 && value1 < 0.72);
        assertTrue(value2 > 0.47 && value2 < 0.48);
        assertTrue(value3 > 0.26 && value3 < 0.27);
    }

    public void testMarketActions(){
        String l_aslpath = "src/main/resources/asl/marketAgent.asl";
        //Set<IAgent<?>> l_agents;
        CMarketAgent agent;
        try
                (
                        final FileInputStream l_stream = new FileInputStream( l_aslpath )
                )
        {
            agent = new CMarketAgent.CMarketAgentGenerator( l_stream,
                    new CMarketAgentManager("marketAgent.asl", null) )
                    .generatesingle(0, "vMarket", "gaussian","FirstPrice");
            final int l_agentNumber = 1;
            /*l_agents =
                    new CMarketAgent.CMarketAgentGenerator( l_stream,
                            new CMarketAgentManager("marketAgent.asl", null) )
                            .generatemultiple( l_agentNumber, 0, "vMarket", "gaussian","FirstPrice")
                            .collect( Collectors.toSet()
                            );*/
        }
        catch ( final Exception l_exception ) {
            l_exception.printStackTrace();
            fail();
            return;
        }

        try {
            agent.call();
            testSetBeliefs(agent);
            agent.call();
            testStartAuction(agent);
            agent.call();
            testReceiveAlertOfAuction(agent);
            agent.call();
            testReceiveBid(agent);
            agent.call();
            testReceiveRoundResult(agent);
            agent.call();
            testReceiveAuctionResult(agent);
            agent.call();
        } catch (Exception e) {
            e.printStackTrace();
            fail();
            throw new RuntimeException();
        }
    }

    public void testBid(){
        String l_aslpath = "src/main/resources/asl/marketAgent.asl";
        CMarketAgent agent;
        try
                (
                        final FileInputStream l_stream = new FileInputStream( l_aslpath )
                )
        {
            agent = new CMarketAgent.CMarketAgentGenerator( l_stream,
                    new CMarketAgentManager("marketAgent.asl", null) )
                    .generatesingle(0, "vMarket", "gaussian","FirstPrice");
        }
        catch ( final Exception l_exception ) {
            l_exception.printStackTrace();
            fail();
            return;
        }

        try {
            agent.call();
            testSetBeliefs(agent);
            agent.call();
            testReceiveAlertOfAuction(agent);
            agent.call();
        } catch (Exception e) {
            e.printStackTrace();
            fail();
        }
    }

    private void testReceiveAlertOfAuction(CMarketAgent agent){
        final ITrigger l_trigger = CTrigger.from(
                ITrigger.EType.ADDGOAL,
                CLiteral.from(
                        "receive/auction",
                        CRawTerm.from(2),
                        CRawTerm.from(MarketConstants.CONTEXT_SPEED),
                        CRawTerm.from(1)
                )
        );
        agent.trigger( l_trigger );
    }

    private void testStartAuction(CMarketAgent agent){
        final ITrigger l_trigger = CTrigger.from(
                ITrigger.EType.ADDGOAL,
                CLiteral.from(
                        "start/auction",
                        CRawTerm.from(MarketConstants.CONTEXT_SPEED)
                )
        );
        agent.trigger( l_trigger );
    }

    private void testReceiveBid(CMarketAgent agent){
        final ITrigger l_trigger = CTrigger.from(
                ITrigger.EType.ADDGOAL,
                CLiteral.from(
                        "receive/bid"
                )
        );
        agent.trigger( l_trigger );
    }

    private void testReceiveRoundResult(CMarketAgent agent){
        final ITrigger l_trigger = CTrigger.from(
                ITrigger.EType.ADDGOAL,
                CLiteral.from(
                        "receive/round/result"
                )
        );
        agent.trigger( l_trigger );
    }

    private void testReceiveAuctionResult(CMarketAgent agent){
        final ITrigger l_trigger = CTrigger.from(
                ITrigger.EType.ADDGOAL,
                CLiteral.from(
                        "receive/result"
                )
        );
        agent.trigger( l_trigger );
    }

    private void testSetBeliefs(CMarketAgent agent){
        ArrayList<Integer> members =  new ArrayList<>();
        IntStream.range(0,5).forEach(i -> members.add(i,i));
        final ITrigger l_trigger = CTrigger.from(
                ITrigger.EType.ADDGOAL,
                CLiteral.from(
                        "setup/beliefs",
                        CRawTerm.from(0),
                        CRawTerm.from(0),
                        CRawTerm.from(100),
                        CRawTerm.from(0),
                        CRawTerm.from(members),
                        CRawTerm.from(75),
                        CRawTerm.from(200)
                )
        );
        agent.trigger( l_trigger );
    }
}


/*if(j == 5){
    ArrayList<Double> can = new ArrayList<>();
    IntStream.range(1,4).forEach(d -> can.add((double)d));
    ArrayList<Double> con = new ArrayList<>();
    con.add(120.0);con.add(130.0); con.add(140.0);
    final ITrigger l_trigger = CTrigger.from(
            ITrigger.EType.ADDGOAL,
            CLiteral.from(
                    "start/vote/regroup",
                    CRawTerm.from(can),
                    CRawTerm.from(con)
            )
    );
    i.trigger( l_trigger );
}*/
/*
IntStream
        .range( 0,20 )
        .forEach( j ->
        l_agents.parallelStream().forEach( i ->{
                try{
                    i.call();
                    switch (j){
                        case 3:{
                            testReceiveAlertOfAuction((CMarketAgent)i);
                            break;
                        }
                        case 6:{
                            testStartAuction((CMarketAgent)i);
                            break;
                        }
                        case 9:{
                            testReceiveBid((CMarketAgent)i);
                            break;
                        }
                        case 12:{
                            testReceiveRoundResult((CMarketAgent)i);
                            break;
                        } case 15:{
                            testReceiveAuctionResult((CMarketAgent)i);
                            break;
                        }case 16:{
                            testSetBeliefs((CMarketAgent)i);
                            break;
                        }
                        default:{
                            break;
                        }
                    }
                }
                catch ( final Exception l_exception )
                {
                    l_exception.printStackTrace();
                    fail();
                    throw new RuntimeException();
                }
            } )
         );
 */