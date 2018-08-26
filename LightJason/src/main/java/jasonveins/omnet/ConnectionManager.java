package jasonveins.omnet;

import javax.annotation.Nonnull;
import java.net.*;
import java.io.*;
import java.nio.ByteBuffer;
import java.util.concurrent.CopyOnWriteArrayList;

public final class ConnectionManager extends Thread {

    private final int port = 4242;
    private ServerSocket serverSocket;
    private Socket clientSocket;
    private PrintWriter out;
    private AgentManager am;

    public boolean isDisconnected() {
        return state == State.DISCONNECTED;
    }

    private enum State{
        DISCONNECTED, WAITINGFORAGENT, RUNNINGLOOP, FINISH, INIT
    }

    private State state = State.INIT;

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

            OutputStream byteStream = clientSocket.getOutputStream();
            byte b[] = new byte[255];
            InputStream stream = clientSocket.getInputStream();

            while ( true ) {

                if( stream.read(b, 0, 255) != -1){
                    byteStream.write(processInput(b));
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

    private byte[] processInput(@Nonnull byte[] b){
        ByteBuffer buffer = ByteBuffer.wrap(b);
        byte[] response;
        int id;
        short size = buffer.getShort();
        if(state == State.FINISH){
            out.println("EndConnection");
            return null;
        }
        switch (buffer.getShort()){
            case Constants.CONNECTION_ACK:
                response = new byte[]{0x00, 0x00, 0x00, 0x06, 0x01, 0x01};
                break;
            case Constants.AGENT_ADD:
                id = buffer.getInt();
                am.createNewAgent(id);
                if(state == State.WAITINGFORAGENT){
                    am.toggleAgentLoop(false, true);
                    state = State.RUNNINGLOOP;
                }
                response = new byte[]{0x00, 0x00, 0x00, 0x06, 0x02, 0x01};
                break;
            case Constants.AGENT_REMOVE:
                id = buffer.getInt();
                am.removeAgent(id);
                response = new byte[]{0x00, 0x00, 0x00, 0x06, 0x03, 0x01};
                break;
            case Constants.UPDATE_SPEED:
                id = buffer.getInt();
                double speed = buffer.getDouble();
                am.updateBeliefs(id, "speed", Double.toString(speed));
                response = new byte[]{0x00, 0x00, 0x00, 0x06, 0x04, 0x01};
                break;
            case Constants.REQUEST_DECISIONS:
                if (am.existsInstructions()) {
                    response = am.extractInstructions();
                } else {
                    response = new byte[]{0x00, 0x00, 0x00, 0x00};
                }

                break;
            case Constants.CONNECTION_END:
                response = new byte[]{0x00, 0x00, 0x00, 0x06, 0x7E, 0x01};
                state = State.DISCONNECTED;
                break;
            default:
                response = new byte[]{0x00};
                break;
        }
        return response;
    }

}
