package jasonveins.omnet.decision;

import java.nio.ByteBuffer;
import java.util.concurrent.CopyOnWriteArrayList;

public class DecisionDataModel {
    protected final CopyOnWriteArrayList<InstructionModel> instructionsList = new CopyOnWriteArrayList<>();

    private int size = 6; //int for length, short for command id

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
        size += iObj.getSize();
    }

    public byte[] convertToMessage(){
        @SuppressWarnings("unchecked")
        CopyOnWriteArrayList<InstructionModel> toSend = (CopyOnWriteArrayList<InstructionModel>)instructionsList.clone();
        instructionsList.removeAll(toSend);
        int messageLength = size;
        size = 6;
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
