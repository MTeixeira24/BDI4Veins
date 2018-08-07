package jasonveins.omnet;

import javax.annotation.Nonnull;
import java.net.*;
import java.io.*;
import java.util.concurrent.CopyOnWriteArrayList;

public final class ConnectionManager extends Thread {

    private final int port = 4242;
    private final String hostname = "localhost";
    private Socket socket;
    private AgentManager am;

    public ConnectionManager(){
        super("ConnectionManager");
    }

    public void setAgentManager(AgentManager m_am){
        am = m_am;
    }

    private void startServer(){
        try {
            ServerSocket serverSocket = new ServerSocket(port);
            Socket clientSocket = serverSocket.accept();
            PrintWriter out =
                    new PrintWriter(clientSocket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(clientSocket.getInputStream()));
            String inputLine, outputLine;

            // Initiate conversation with client
            JasoNetProtocol jnp = new JasoNetProtocol();
            String action[];
            //outputLine = kkp.processInput(null);
            //out.println(outputLine);
            while ( true ) {
                if((inputLine = in.readLine()) != null){
                    action = inputLine.split("-");
                    switch (action[0]) {
                        case "Add":
                            am.createNewAgent(Integer.parseInt(action[1]));
                            outputLine = "Agent Added";
                            out.println(outputLine);
                            break;
                        case "BeliefUpdate":
                            am.updateBeliefs(Integer.parseInt(action[1]), action[2], action[3]);
                            outputLine = "Beliefs updated";
                            out.println(outputLine);
                            break;
                        case "Query":
                            if (am.existsInstructions()) {
                                outputLine = "";
                                CopyOnWriteArrayList<String> inst = am.extractInstructions();
                                for (int i = 0; i < inst.size(); i++) {
                                    outputLine = outputLine.concat(inst.get(i) + ":");
                                }
                                out.println(outputLine);
                            } else {
                                outputLine = "Ok";
                                out.println(outputLine);
                            }
                            break;
                        default:
                            outputLine = "Ok";
                            out.println(outputLine);
                            break;
                    }
                }else{
                    break;
                }
            }
        } catch (IOException e) {
            System.out.println("Exception caught when trying to listen on port "
                    + port + " or listening for a connection");
            System.out.println(e.getMessage());
        }
    }

    public void run(){
        startServer();
    }

}
