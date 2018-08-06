package trafficproject;

import javax.annotation.Nonnull;
import java.io.FileInputStream;
import java.util.Collections;
import java.util.Set;
import java.util.logging.LogManager;
import java.util.stream.Collectors;
import java.util.stream.IntStream;


/**
 * main application with runtime
 */
final class App
{

    static
    {
        // disable logger
        LogManager.getLogManager().reset();
    }

    /**
     * private constructor to avoid any instantiation
     */
    private App()
    {
    }


    /**
     * main method
     *
     * @param p_args command-line arguments
     */
    public static void main( final String[] p_args )
    {
        if ( p_args.length < 2 )
            throw new RuntimeException( "arguments are not set: ASL script, number of agents" );

        // parameter of the command-line arguments:
        // 1. ASL file
        // 2. number of agents
        // 3. number of iterations (if not set maximum integer)
       /* final Set<MyAgent> l_agents;
        try
            (
                final FileInputStream l_stream = new FileInputStream( p_args[0] );
            )
        {
            // agent factory with generating an unmodifiable set
            l_agents = Collections.unmodifiableSet(
                new MyAgentGenerator( l_stream )
                    .generatemultiple( Integer.parseInt( p_args[1] ) )
                    .collect( Collectors.toSet() )
            );
        }
        catch ( final Exception l_exception )
        {
            l_exception.printStackTrace();
            return;
        }

        // runtime call (with parallel execution)
        execute(l_agents, p_args);*/

    }

    final static void execute(@Nonnull final Set<MyAgent> l_agents, final String[] p_args){
        IntStream
                .range(
                        0,
                        p_args.length < 3
                                ? Integer.MAX_VALUE
                                : Integer.parseInt( p_args[2] )
                )
                .forEach( j -> l_agents.parallelStream()
                        .forEach( i ->
                        {
                            try
                            {
                                i.call();
                            }
                            catch ( final Exception l_exception )
                            {
                                l_exception.printStackTrace();
                                throw new RuntimeException();
                            }
                        } ) );
    }
}
