package jasonveins;

import jasonveins.omnet.managers.AgentManager;
import jasonveins.omnet.managers.CVoterAgentManager;
import jasonveins.omnet.managers.ConnectionManager;


import java.io.FileNotFoundException;
import java.io.PrintStream;
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
    public static void main( final String[] p_args ) {

        ConnectionManager cm = new ConnectionManager();
        cm.start();
    }
}
