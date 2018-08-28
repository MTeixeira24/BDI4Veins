package jasonveins;

import jasonveins.omnet.AgentManager;
import jasonveins.omnet.ConnectionManager;


import java.util.logging.LogManager;


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
        ConnectionManager cm = new ConnectionManager();
        AgentManager am = new AgentManager(/*"manager.asl"*/"laneMergeVehicle.asl", cm);
        cm.setAgentManager(am);
        cm.start();
        am.loop();
    }
}
