package jasonveins.omnet.managers;

import jasonveins.omnet.managers.constants.Constants;

import javax.annotation.Nonnull;
import java.net.*;
import java.io.*;
import java.nio.ByteBuffer;
import java.util.concurrent.CountDownLatch;

public final class ConnectionManager extends Thread {

    private final int port = 4242;
    private ServerSocket serverSocket;
    private Socket clientSocket;
    private PrintWriter out;
    private AgentManager am;
    private CountDownLatch am_latch;

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
        try {
            clientSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void setAgentManager(AgentManager m_am){
        am = m_am;
        am_latch = am.getLatch();
    }

    private void startServer() throws IOException {
        int bufferSize = 1024*1024;
        //***************************************/
        while(true){
            //am = new CVoterAgentManager("iterativeVoter.asl", this);
            //am_latch = am.getLatch();
            //am.start();
            try{
                serverSocket = new ServerSocket(port);
                clientSocket = serverSocket.accept();
                state = State.WAITINGFORAGENT;
                out = new PrintWriter(clientSocket.getOutputStream(), true);

                OutputStream byteStream = clientSocket.getOutputStream();
                byte b[] = new byte[bufferSize];
                InputStream stream = clientSocket.getInputStream();

                while ( true ) {

                    if( stream.read(b, 0, bufferSize) != -1){
                        //int size = ByteBuffer.wrap(b).getInt();
                        byteStream.write(processInput(b));
                        if(state == State.FINISH) break;
                    }else{
                        System.out.println("Terminating connection");
                        state = State.DISCONNECTED;
                        if(am.getAgentLoopStatus())
                            am.toggleAgentLoop(true, false);
                        //System.exit(0);
                        break;
                    }

                }
            } catch(SocketException e){
                System.out.println("Connection termination: Ending agent loop");
                state = State.DISCONNECTED;
                if(am.getAgentLoopStatus())
                    am.toggleAgentLoop(true, false);
                //System.exit(0);
            } catch (IOException e) {
                System.out.println("Exception caught when trying to listen on port "
                        + port + " or listening for a connection");
                e.printStackTrace();
                state = State.DISCONNECTED;
                if(am.getAgentLoopStatus())
                    am.toggleAgentLoop(true, false);
                //System.exit(0);
            } catch (NullPointerException e){
                serverSocket.close();
                clientSocket.close();
                out.close();

            }finally {
                serverSocket.close();
                clientSocket.close();
                out.close();
            }
        }
        //***************************************/

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
        int size = buffer.getInt();
        byte[] response;
        int id;
        if(state == State.FINISH){
            out.println("EndConnection");
            return new byte[]{0x00, 0x00, 0x00, 0x06, 0x7E, 0x01};
        }
        size -= 2;
        switch (buffer.getShort()){
            case Constants.CONNECTION_ACK:
                response = new byte[]{0x00, 0x00, 0x00, 0x06, 0x01, 0x01};
                break;
            case Constants.BULK_AGENT_ADD:
                response = am.bulkCreateAgents(buffer.slice());
                if(state == State.WAITINGFORAGENT){
                    //am.toggleAgentLoop(false, true);
                    state = State.RUNNINGLOOP;
                    am_latch.countDown();
                }
                break;
            case Constants.AGENT_ADD:
                id = buffer.getInt();
                String vType = CByteUtils.extractString(buffer);
                String aslFile = CByteUtils.extractString(buffer);
                am.createNewAgent(id, vType, aslFile);
                if(state == State.WAITINGFORAGENT){
                    //am.toggleAgentLoop(false, true);
                    state = State.RUNNINGLOOP;
                    am_latch.countDown();
                }
                response = new byte[]{0x00, 0x00, 0x00, 0x06, 0x02, 0x01};
                break;
            case Constants.AGENT_REMOVE:
                id = buffer.getInt();
                try {
                    am.removeAgent(id);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                    System.exit(2);
                }
                response = new byte[]{0x00, 0x00, 0x00, 0x06, 0x03, 0x01};
                break;
            case Constants.SET_BELIEF:
                response = am.bulkTrigger(buffer.slice());
                /*id = buffer.getInt();
                size -= 4;
                String belief = CByteUtils.extractString(buffer);
                size -= belief.length();
                am.updateGoals(id, belief ,buffer.slice(), size);*/
                //response = new byte[]{0x00, 0x00, 0x00, 0x06, 0x04, 0x01};
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
            case Constants.SET_SIM_PARAMS:{
                String managerType = CByteUtils.extractString(buffer);
                createManager(managerType);


                am_latch = am.getLatch();
                am.start();
                am.setSimParams(buffer);
                //am.setSimParams(platoonSize, iteration, rule, type, factor, utility, committee_vote_rule);
                response = new byte[]{0x00, 0x00, 0x00, 0x06, 0x06, 0x01};
                break;
            }
            default:
                response = new byte[]{0x00};
                break;
        }
        return response;
    }

    private void createManager(@notnull String managerType){
        switch (managerType){
            case "AgentManager":
                am = new AgentManager("vehicle.asl", this);
                break;
            case "CVoterAgentManager":
                am = new CVoterAgentManager("voterAgent.asl", this);
                break;
            case "CRouterManager":
                am = new CRouterManager("router.asl", this);
                break;
            case "CMarketAgentManager":
                am = new CMarketAgentManager("marketAgent.asl", this);
                break;
            case "CBargainAgentManager":
                am = new CBargainAgentManager("bargainAgent.asl", this);
                break;
            default:
                throw new RuntimeException("ConnectionManager: Invalid manager specified!");
        }
    }

}
