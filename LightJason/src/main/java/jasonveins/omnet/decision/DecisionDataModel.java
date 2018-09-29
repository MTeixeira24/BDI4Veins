package jasonveins.omnet.decision;

import java.nio.ByteBuffer;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicInteger;

public class DecisionDataModel {
    protected final CopyOnWriteArrayList<InstructionModel> instructionsList = new CopyOnWriteArrayList<>();

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
        instructionsList.add(iObj);
        //size.addAndGet(iObj.getSize());
        //size += iObj.getSize();
    }

    public byte[] convertToMessage(){
        @SuppressWarnings("unchecked")
        CopyOnWriteArrayList<InstructionModel> toSend = (CopyOnWriteArrayList<InstructionModel>)instructionsList.clone();
        instructionsList.removeAll(toSend);
        //Calculate message length from toSendList
        int messageLength = 6;
        for(InstructionModel im : toSend){
            messageLength += im.getSize();
        }
        //int messageLength = size.getAndSet(6);
        ByteBuffer bb = ByteBuffer.allocate(messageLength);
        bb.putInt(messageLength); bb.putShort((short)5);
        for(InstructionModel io : toSend){
            byte[] instruction = io.toByteArray();
            bb.put(instruction);
        }
        return bb.array();
    }

    public boolean isEmpty() {
        return instructionsList.isEmpty();
    }
}
