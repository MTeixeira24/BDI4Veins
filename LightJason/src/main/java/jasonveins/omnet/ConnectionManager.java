package jasonveins.omnet;

import javax.annotation.Nonnull;
import java.net.*;
import java.io.*;
import java.util.concurrent.CopyOnWriteArrayList;

public final class ConnectionManager extends Thread {

    private final int port = 4242;
    private final String hostname = "localhost";
    private ServerSocket serverSocket;
    private Socket clientSocket;
    private PrintWriter out;
    private AgentManager am;

    public ConnectionManager(){
        super("ConnectionManager");
    }

    public void setAgentManager(AgentManager m_am){
        am = m_am;
    }

    private void startServer() throws IOException {
        try{
            serverSocket = new ServerSocket(port);
            clientSocket = serverSocket.accept();
            out = new PrintWriter(clientSocket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(clientSocket.getInputStream()));
            String inputLine;
            while ( true ) {
                if((inputLine = in.readLine()) != null){
                    processInput(inputLine);
                }else{
                    break;
                }
            }
        } catch (IOException e) {
            System.out.println("Exception caught when trying to listen on port "
                    + port + " or listening for a connection");
            System.out.println(e.getMessage());
        }finally {
            serverSocket.close();
            clientSocket.close();
            out.close();
        }
    }

    public void run(){
        try {
            startServer();
        } catch (IOException e) {
            System.out.println("Exception caught when trying to listen on port "
                    + port + " or listening for a connection");
            e.printStackTrace();
        }
    }

    private void processInput(@Nonnull String inputLine){
        String [] action = inputLine.split("-");
        String outputLine;
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
                    for (String anInst : inst) {
                        outputLine = outputLine.concat(anInst + ":");
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
    }

}
