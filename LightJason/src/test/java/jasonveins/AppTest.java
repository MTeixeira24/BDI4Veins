package jasonveins;

import jasonveins.omnet.agent.CRouterAgent;
import jasonveins.omnet.managers.CRouterManager;
import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;
import org.lightjason.agentspeak.agent.IAgent;

import java.io.FileInputStream;
import java.util.Set;
import java.util.stream.Collectors;

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
        String l_aslpath = "src/main/resources/asl/router.asl";
        Set<IAgent<?>> l_agents;
        try
                (
                        final FileInputStream l_stream = new FileInputStream( l_aslpath ) //TODO: Get rid of this
                )
        {
            final int l_agentNumber = 0;
            l_agents =
                    new CRouterAgent.CRouterAgentGenerator( l_stream,
                            new CRouterManager("router.asl", null) )
                            .generatemultiple( l_agentNumber, 0 )
                            .collect( Collectors.toSet()

                            );
        }
        catch ( final Exception l_exception )
        {
            l_exception.printStackTrace();
            return;
        }
        assertTrue( true );
    }
}
