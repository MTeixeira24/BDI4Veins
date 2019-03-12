package jasonveins.omnet.decision;

import java.nio.ByteBuffer;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicInteger;

public class DecisionDataModel {
    protected final CopyOnWriteArrayList<InstructionModel> instructionsList = new CopyOnWriteArrayList<>();
    private final ConcurrentLinkedQueue<InstructionModel> instructionsQueue = new ConcurrentLinkedQueue<>();

    //private AtomicInteger size = new AtomicInteger(6); //int for length, short for command id

    public DecisionDataModel(){
        /*//Simulate an agent creating a request
        InstructionModel iOb = new InstructionModel(8, 4);
        iOb.pushInt(3);
        //iOb.pushDouble(2.991);
        //assert iOb.getSize() == 28;
        addInstruction(iOb);*/
    }

    public void addInstruction(InstructionModel iObj){
        //instructionsList.add(iObj);
        instructionsQueue.offer(iObj);
    }

    public byte[] convertToMessage2(){

        CopyOnWriteArrayList<InstructionModel> toSend = (CopyOnWriteArrayList<InstructionModel>)instructionsList.clone();
        instructionsList.removeAll(toSend);
        //Calculate message length from toSendList
        int messageLength = 6;


        for(InstructionModel im : toSend){
            messageLength += im.getSize();
        }
        ByteBuffer bb = ByteBuffer.allocate(messageLength);
        //int messageLength = size.getAndSet(6);
        bb.putInt(messageLength); bb.putShort((short)5);
        for(InstructionModel io : toSend){
            byte[] instruction = io.toByteArray();
            bb.put(instruction);
        }

        return bb.array();
    }

    public byte[] convertToMessage(){

        //CopyOnWriteArrayList<InstructionModel> toSend = (CopyOnWriteArrayList<InstructionModel>)instructionsList.clone();
        //instructionsList.removeAll(toSend);
        int bufferSize = 256;
        //Calculate message length from toSendList
        int messageLength = 6;
        int size = instructionsQueue.size();
        ByteBuffer bb = ByteBuffer.allocate(bufferSize*size);
        bb.putInt(messageLength); bb.putShort((short)5);
        for(int i = 0; i < size; i++){
            InstructionModel io = instructionsQueue.poll();
            messageLength += io.getSize();
            byte[] instruction = io.toByteArray();
            bb.put(instruction);
        }
        bb.putInt(0, messageLength);

        byte[] byteArray = new byte[messageLength];
        System.arraycopy(bb.array(), 0, byteArray, 0, messageLength - 1);

        //for(InstructionModel im : toSend){
        //    messageLength += im.getSize();
        //}
        //int messageLength = size.getAndSet(6);
        //ByteBuffer bb = ByteBuffer.allocate(messageLength);
        /*bb.putInt(messageLength); bb.putShort((short)5);
        for(InstructionModel io : toSend){
            byte[] instruction = io.toByteArray();
            bb.put(instruction);
        }*/

        return byteArray;
    }

    public boolean isEmpty() {
        return instructionsList.isEmpty();
    }
}
