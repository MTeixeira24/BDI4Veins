package jasonveins.experiments;

import org.lightjason.agentspeak.agent.IAgent;
import jasonveins.MyAgentGenerator;
import jasonveins.environment.Link;
import jasonveins.environment.Node;

import jasonveins.omnet.*;

import java.io.*;
import java.net.UnknownHostException;
import java.util.Collections;
import java.util.Set;
import java.util.logging.LogManager;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

/*Sockets based imports*/
import java.net.Socket;


final class Main {
    static
    {
        // disable logger
        LogManager.getLogManager().reset();
    }
    public static void main( final String[] p_args ){

        //agentTest(p_args);
        //socketsTest(/*p_args*/);
        startInterface(p_args[0]);
    }

    public static void startInterface(String aslPath){
        ConnectionManager cm = new ConnectionManager();
        AgentManager am = new AgentManager("manager.asl", cm);
        cm.setAgentManager(am);
        cm.start();
        /*final Set<IAgent<?>> l_agents;
        try
                (
                        final FileInputStream l_stream = new FileInputStream( "manager.asl" )
                )
        {
            final int l_agentnumber = 1;
            l_agents = Collections.unmodifiableSet(
                    new ManagerGenerator( l_stream )
                            .generatemultiple( l_agentnumber )
                            .collect( Collectors.toSet() )

            );
        }
        catch ( final Exception l_exception )
        {
            l_exception.printStackTrace();
            return;
        }*/
        am.loop();
        /*
        IntStream
                .range(0,8000)
                .forEach( j -> am.agents().parallelStream()
                        .forEach( i ->
                        {
                            if(j == 7999){
                                System.out.println("Helo");
                            }
                            try
                            {
                                i.call();
                            }
                            catch ( final Exception l_exception )
                            {
                                l_exception.printStackTrace();
                                throw new RuntimeException();
                            }
                        } ) );*/

    }

    public static void socketsTest(/*final String[] p_args*/){
        String hostName = "localhost";
        int portNumber = 8080;

        try(
                Socket socket = new Socket(hostName, portNumber);
                PrintWriter writer = new PrintWriter(socket.getOutputStream(), true);
                BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in)) //Read user input
                )
        {
            String userInput;
            while((userInput = stdIn.readLine()) != null){
                writer.println(userInput);
                System.out.println("Echo: " + reader.readLine());
            }
        } catch (UnknownHostException e) {
            System.err.println("Don't know about host " + hostName);
            e.printStackTrace();
            System.exit(1);
        } catch (IOException e) {
            System.err.println("Couldn't get I/O for the connection to " + hostName);
            e.printStackTrace();
            System.exit(1);
        }

}

    public static void agentTest(final String[] p_args){
        System.out.println("Creating a network with two nodes, one link and two lanes");
        Node nodeA = new Node("A");
        Node nodeB = new Node("B");
        Link AB = new Link(nodeA, nodeB, 2, 0, 30);
        System.out.println("Success");

        System.out.println("Creating one agent to make it run around in the lane");
        final Set<IAgent<?>> l_agents;

        try
                (
                        // stream with ASL code
                        final FileInputStream l_stream = new FileInputStream( p_args[0] )
                ) /*This closes the file stream at the end of the try block*/
        {
            final int l_agentnumber = 2;

            l_agents = Collections.unmodifiableSet(

                    // create agent generator with environment reference
                    new MyAgentGenerator( l_stream, AB.lanes().get(0) )
                            .generatemultiple( l_agentnumber )
                            .collect( Collectors.toSet() )

            );
        }
        catch ( final Exception l_exception )
        {
            l_exception.printStackTrace();
            return;
        }
        System.out.println("Success");
        System.out.println("Have the agent move until end of lane");
        IntStream
                .range(0,8000)
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
