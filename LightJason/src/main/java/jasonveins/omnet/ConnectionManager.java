package jasonveins.omnet;

import javax.annotation.Nonnull;
import java.net.*;
import java.io.*;
import java.util.concurrent.CopyOnWriteArrayList;

public final class ConnectionManager extends Thread {

    private final int port = 4242;
    private final String hostname = "localhost";
    private Socket socket;
    protected AgentManager am;

    public ConnectionManager(/*AgentManager m_am*/){
        super("ConnectionManager");
        //am = m_am;

        /*socket = establishConnection();
        if(socket == null){
            throw new ExceptionInInitializerError();
        }*/
        /* //Set a socket to listen for incoming connections
        try (
                ServerSocket serverSocket = new ServerSocket(port);
                Socket clientSocket = serverSocket.accept();
                PrintWriter out =
                        new PrintWriter(clientSocket.getOutputStream(), true);
                BufferedReader in = new BufferedReader(
                        new InputStreamReader(clientSocket.getInputStream()));
        ) {

            String inputLine, outputLine;

            // Initiate conversation with client
            JasoNetProtocol jnp = new JasoNetProtocol();
            //outputLine = kkp.processInput(null);
            //out.println(outputLine);

            while ((inputLine = in.readLine()) != null) {
                outputLine = jnp.processInput(inputLine);
                out.println(outputLine);
                if (outputLine.equals("Bye."))
                    break;
            }
        } catch (IOException e) {
            System.out.println("Exception caught when trying to listen on port "
                    + port + " or listening for a connection");
            System.out.println(e.getMessage());
        }*/
    }

    public void setAgentManager(AgentManager m_am){
        am = m_am;
    }

    private void startServer(){
        try {
            ServerSocket serverSocket = new ServerSocket(port);
            int test = 0;
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
                    if(action[0].equals("Add")){
                        am.createNewAgent(Integer.parseInt(action[1]));
                        outputLine = "Agent Added";
                        out.println(outputLine);
                    }else if(action[0].equals("BeliefUpdate")){
                        am.updateBeliefs(Integer.parseInt(action[1]), action[2], action[3]);
                        outputLine = "Beliefs updated";
                        out.println(outputLine);
                    }else if(action[0].equals("Query")){
                        if(am.existsInstructions()){
                            outputLine = "";
                            CopyOnWriteArrayList<String> inst = am.extractInstructions();
                            for (int i = 0; i < inst.size(); i++) {
                                outputLine = outputLine.concat(inst.get(i) + ":");
                            }
                            out.println(outputLine);
                        }else{
                            outputLine = "Ok";
                            out.println(outputLine);
                        }
                    }else{
                        outputLine = "Ok";
                        out.println(outputLine);
                    }
                    /*if (outputLine.equals("end"))
                        break;*/
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
        //listenConnection();
        startServer();
    }

    private void listenConnection(){
        String msg;
        try(
                BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream()))
                ){
            while(true){
                if(reader.ready()){
                    msg = reader.readLine();
                    System.out.println("Received message: " + msg);
                    if(msg.equals("end"))
                        break;
                    else{
                        sendMessage("OK");
                    }
                }
            }
        }catch (IOException e){
            System.err.println("Couldn't get I/O for the connection to " + hostname);
            e.printStackTrace();
            System.exit(1);
        }
    }

    public void sendMessage(@Nonnull String msg){
        try(
                PrintWriter writer = new PrintWriter(socket.getOutputStream(), true)
        ){
            System.out.println("Sending message: " + msg);
            writer.println(msg);
        }catch (IOException e){
            System.err.println("Couldn't get I/O for the connection to " + hostname);
            e.printStackTrace();
            System.exit(1);
        }
    }

    private Socket establishConnection(){
        try{
            return new Socket(hostname, port);
        }catch (UnknownHostException e) {
            System.err.println("Don't know about host " + hostname);
            e.printStackTrace();
            System.exit(1);
        } catch (IOException e) {
            System.err.println("Couldn't get I/O for the connection to " + hostname);
            e.printStackTrace();
            System.exit(1);
        }
        return null;
    }

}
