package jasonveins.featureTest;

import javax.annotation.Nonnull;
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.concurrent.CopyOnWriteArrayList;

class DecisionDataModel{

    class ObjectPair{
        private short valueType;
        private byte[] value;

        public ObjectPair(short _valueType, byte[] _value){
            valueType = _valueType;
            value = _value;
        }

        public short valueType(){
            return valueType;
        }

        public byte[] value(){
            return value;
        }
    }

    class InstructionObject{
        private int size;
        private int agentId;
        private int agentAction;
        private ArrayList<ObjectPair> values = new ArrayList<>();

        public InstructionObject(int _agentId, int _agentAction) {
            agentId = _agentId;
            agentAction = _agentAction;
            size = 12;
        }

        public int getSize() {
            return size;
        }

        public int getAgentId() {
            return agentId;
        }

        public int getAgentAction() {
            return agentAction;
        }

        public void pushChar(char _value){
            short type = 2;
            values.add(new ObjectPair(type, ByteBuffer.allocate(1).putChar(_value).array()));
            size += 4; //two bytes for valueType, remainder for data
        }
        public void pushShort(short _value){
            short type = 3;
            values.add(new ObjectPair(type, ByteBuffer.allocate(2).putShort(_value).array()));
            size += 4;
        }
        public void pushInt(int _value){
            short type = 4;
            values.add(new ObjectPair(type, ByteBuffer.allocate(4).putInt(_value).array()));
            size += 6;
        }
        public void pushLong(long _value){
            short type = 5;
            values.add(new ObjectPair(type, ByteBuffer.allocate(8).putLong(_value).array()));
            size += 10;
        }
        public void pushFloat(float _value){
            short type = 6;
            values.add(new ObjectPair(type, ByteBuffer.allocate(4).putFloat(_value).array()));
            size += 6;
        }
        public void pushDouble(double _value){
            short type = 7;
            values.add(new ObjectPair(type, ByteBuffer.allocate(8).putDouble(_value).array()));
            size += 10;
        }

        public byte[] toByteArray(){
            ByteBuffer bb = ByteBuffer.allocate(this.size);
            bb.putInt(this.size);
            bb.putInt(getAgentId());
            bb.putInt(getAgentAction());
            for(ObjectPair op : values){
                bb.putShort(op.valueType());
                bb.put(op.value());
            }
            return bb.array();
        }
    }

    protected final CopyOnWriteArrayList<InstructionObject> instructionsList = new CopyOnWriteArrayList<>();

    private int size = 6; //int for length, short for command id

    public DecisionDataModel(){
        //Simulate an agent creating a request
        InstructionObject iOb = new InstructionObject(8, 4);
        iOb.pushInt(3);
        //iOb.pushDouble(2.991);
        //assert iOb.getSize() == 28;
        addInstruction(iOb);
    }

    public void addInstruction(InstructionObject iObj){
        instructionsList.add(iObj);
        size += iObj.getSize();
    }

    public byte[] convertToMessage(){
        @SuppressWarnings("unchecked")
        CopyOnWriteArrayList<InstructionObject> toSend = (CopyOnWriteArrayList<InstructionObject>)instructionsList.clone();
        instructionsList.removeAll(toSend);
        int messageLength = size;
        ByteBuffer bb = ByteBuffer.allocate(messageLength);
        bb.putInt(messageLength); bb.putShort((short)5);
        for(InstructionObject io : toSend){
            byte[] instruction = io.toByteArray();
            bb.put(instruction);
        }
        return bb.array();
    }


}

public class ByteSocket {

    private static final int port = 4242;
    private static boolean connected = true;
    private static DecisionDataModel ddm = new DecisionDataModel();

    public static void main( final String[] p_args )
    {
        try(ServerSocket serverSocket = new ServerSocket(port);
            Socket clientSocket = serverSocket.accept();
            PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true))
        {
            OutputStream byteStream = clientSocket.getOutputStream();
            byte b[] = new byte[255];
            InputStream stream = clientSocket.getInputStream();
            while(connected){
                stream.read(b, 0, 255);
                byteStream.write(processInput(b));
                //out.println(processInput(b));
            }
        } catch(SocketException e){
            System.err.println("Abrupt connection termination! Ending agent loop");
            System.exit(0);
        } catch (IOException e) {
            System.out.println("Exception caught when trying to listen on port "
                    + port + " or listening for a connection");
            e.printStackTrace();
            System.exit(0);
        }
    }

    private static byte[] processInput(@Nonnull byte[] b){
        ByteBuffer buffer = ByteBuffer.wrap(b);
        byte[] response;
        int id;
        short size = buffer.getShort();
        System.out.println("The message has " + size + " bytes");
        switch (buffer.getShort()){
            case 1:
                System.out.println("The action is acknowledge connection");
                response = new byte[]{0x01, 0x01};
                break;
            case 2:
                id = buffer.getInt();
                System.out.println("The action is to add an agent of id " + id);
                response = new byte[]{0x02, 0x01};
                break;
            case 3:
                id = buffer.getInt();
                System.out.println("The action is to remove an agent of id " + id);
                response = new byte[]{0x03, 0x01};
                break;
            case 4:
                System.out.println("The action is to set the max speed ");
                id = buffer.getInt();
                System.out.println("With an id of " + id);
                double speed = buffer.getDouble();
                System.out.println("And a new speed value of " + speed);
                response = new byte[]{0x04, 0x01};
                break;
            case 5:
                System.out.println("Client requests the decisions");
                //response = new byte[]{0x05, 0x01};
                response = ddm.convertToMessage();
                break;
            case 126:
                System.out.println("The action is to end communications");
                response = new byte[]{0x7E, 0x01};
                connected = false;
                break;
            default:
                response = new byte[]{0x00};
                break;
        }
        return response;
    }
}
