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

    private enum State{
        DISCONNECTED, WAITINGFORAGENT, RUNNINGLOOP, FINISH
    }

    private State state = State.DISCONNECTED;

    public ConnectionManager(){
        super("ConnectionManager");
    }

    public void finish() {
        state = State.FINISH;
    }

    public void setAgentManager(AgentManager m_am){
        am = m_am;
    }

    private void startServer() throws IOException {
        try{
            serverSocket = new ServerSocket(port);
            clientSocket = serverSocket.accept();
            state = State.WAITINGFORAGENT;
            out = new PrintWriter(clientSocket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(clientSocket.getInputStream()));
            String inputLine;
            while ( true ) {
                if((inputLine = in.readLine()) != null){
                    processInput(inputLine);
                    if(state == State.FINISH) break;
                }else{
                    System.out.println("Terminating connection");
                    state = State.DISCONNECTED;
                    am.toggleAgentLoop(true, false);
                    System.exit(0);
                    break;
                }
            }
        } catch(SocketException e){
            System.err.println("Abrupt connection termination! Ending agent loop");
            state = State.DISCONNECTED;
            am.toggleAgentLoop(true, false);
            System.exit(0);
        } catch (IOException e) {
            System.out.println("Exception caught when trying to listen on port "
                    + port + " or listening for a connection");
            e.printStackTrace();
            state = State.DISCONNECTED;
            am.toggleAgentLoop(true, false);
            System.exit(0);
        } finally {
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
        if(state == State.FINISH){
            out.println("EndConnection");
            return;
        }
        switch (action[0]) {
            case "Add":
                am.createNewAgent(Integer.parseInt(action[1]));
                outputLine = "Agent Added";
                if(state == State.WAITINGFORAGENT){
                    am.toggleAgentLoop(false, true);
                    state = State.RUNNINGLOOP;
                }
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
            case "Remove":
                am.removeAgent(Integer.parseInt(action[1]));
                outputLine = "Agent removed";
                out.println(outputLine);
                break;
            default:
                outputLine = "Ok";
                out.println(outputLine);
                break;
        }
    }

}
