package jasonveins;

import jasonveins.omnet.agent.CRegroupVoter;
import jasonveins.omnet.agent.CRouterAgent;
import jasonveins.omnet.agent.CVoterAgentGenerator;
import jasonveins.omnet.managers.CRouterManager;
import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;
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
    public void testApp()
    {
        String l_aslpath = "src/main/resources/asl/iterativeVoter.asl";
        Set<IAgent<?>> l_agents;
        try
                (
                        final FileInputStream l_stream = new FileInputStream( l_aslpath )
                )
        {
            final int l_agentNumber = 1;
            l_agents =
                    new CVoterAgentGenerator( l_stream,
                            new CRouterManager("iterativeVoter.asl", null) )
                            .generatemultiple( l_agentNumber, 0, "vVoter",
                                    "Approval", 0.05, "khan", "Minimax" )
                            .collect( Collectors.toSet()

                            );
        }
        catch ( final Exception l_exception )
        {
            l_exception.printStackTrace();
            fail();
            return;
        }
        IntStream
                // define cycle range, i.e. number of cycles to run sequentially
                .range( 0,20 )
                .forEach( j ->
                {
                    //System.out.println( "Global cycle: " + j );
                    l_agents.parallelStream().forEach( i ->
                    {
                        try
                        {
                            i.call();
                            if(j == 5){
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
                            }
                        }
                        catch ( final Exception l_exception )
                        {
                            l_exception.printStackTrace();
                            fail();
                            throw new RuntimeException();
                        }
                    } );
                } );
    }

    /*public void testRouter(){
        String l_aslpath = "src/main/resources/asl/router.asl";
        Set<IAgent<?>> l_agents;
        try
                (
                        final FileInputStream l_stream = new FileInputStream( l_aslpath )
                )
        {
            final int l_agentNumber = 1;
            l_agents =
                    new CRouterAgent.CRouterAgentGenerator( l_stream,
                            new CRouterManager("router.asl", null) )
                            .generatemultiple( l_agentNumber, 0, "vRouter" )
                            .collect( Collectors.toSet()

                            );
        }
        catch ( final Exception l_exception )
        {
            l_exception.printStackTrace();
            fail();
            return;
        }
        IntStream
                // define cycle range, i.e. number of cycles to run sequentially
                .range( 0,20 )
                .forEach( j ->
                {
                    //System.out.println( "Global cycle: " + j );
                    l_agents.parallelStream().forEach( i ->
                    {
                        try
                        {
                            i.call();
                        }
                        catch ( final Exception l_exception )
                        {
                            l_exception.printStackTrace();
                            fail();
                            throw new RuntimeException();
                        }
                    } );
                } );
    }*/
}
