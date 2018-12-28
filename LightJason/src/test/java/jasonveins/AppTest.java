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
import org.lightjason.agentspeak.agent.IBaseAgent;
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

    private ArrayList<CMarketAgent> createAgentList(){
        String l_aslpath = "src/main/resources/asl/marketAgent.asl";
        ArrayList<CMarketAgent> agents = new ArrayList<>(5);
        CMarketAgentManager am = new CMarketAgentManager("marketAgent.asl", null);
        for(int i = 0; i < 5; i++){
            try(
                final FileInputStream l_stream = new FileInputStream( l_aslpath )
            ){
                agents.add(new CMarketAgent.CMarketAgentGenerator( l_stream,am)
                        .generatesingle(i, "vMarket", "gaussian","FirstPrice"));

            }catch ( final Exception l_exception ) {
                l_exception.printStackTrace();
                fail();
            }
        }
        return agents;
    }

    private void setAgentsBeliefs(ArrayList<CMarketAgent> agents){

    }

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

    public void testStartAuction(){
        ArrayList<CMarketAgent> agents = createAgentList();

        try {
            agents.get(0).call();
            agents.forEach(a -> testSetBeliefs(a, a.id()));
            agents.get(0).call();
            testStartAuction(agents.get(0));
            agents.get(0).call();
        } catch (Exception e) {
            e.printStackTrace();
            fail();
            throw new RuntimeException();
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

    private void testSetBeliefs(CMarketAgent agent, int id){
        ArrayList<Integer> members =  new ArrayList<>();
        IntStream.range(0,5).forEach(i -> members.add(i,i));
        ArrayList<Integer> route =  new ArrayList<>();
        route.add(2); route.add(4); route.add(7); route.add(8);
        final ITrigger l_trigger = CTrigger.from(
                ITrigger.EType.ADDGOAL,
                CLiteral.from(
                        "setup/beliefs",
                        CRawTerm.from(0),
                        CRawTerm.from(0),
                        CRawTerm.from(100),
                        CRawTerm.from(0),
                        CRawTerm.from(members),
                        CRawTerm.from(40 + 5*id),
                        CRawTerm.from(200),
                        CRawTerm.from(route),
                        CRawTerm.from(80+5*id)
                )
        );
        agent.trigger( l_trigger );
    }
}