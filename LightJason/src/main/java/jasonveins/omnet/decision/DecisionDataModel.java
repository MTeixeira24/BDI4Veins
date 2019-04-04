package jasonveins.omnet.decision;

import java.nio.ByteBuffer;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicInteger;

public class DecisionDataModel {
    private final ConcurrentLinkedQueue<InstructionModel> instructionsQueue = new ConcurrentLinkedQueue<>();
    private int agentsToReply;
    private AtomicInteger agentsThatReplied;

    public DecisionDataModel(){
        agentsToReply = -1;
        agentsThatReplied = new AtomicInteger(0);
    }

    public boolean addInstruction(InstructionModel iObj){
        instructionsQueue.offer(iObj);
        incrementRepliedAgents();
        return (agentsThatReplied.get() == agentsToReply);
    }

    public boolean handleReturnCode(int code){
        incrementRepliedAgents();
        return (agentsThatReplied.get() == agentsToReply);

    }

    private void incrementRepliedAgents(){
        while(true) {
            int existingValue = agentsThatReplied.get();
            int newValue = existingValue + 1;
            if(agentsThatReplied.compareAndSet(existingValue, newValue)) {
                return;
            }
        }
    }

    public boolean allReplied(){
        return (agentsThatReplied.get() == agentsToReply);
    }

    public void setAgentCount(int agents){
        agentsToReply = agents;
        agentsThatReplied.set(0);
    }

    public byte[] convertToMessage(){
        int size = instructionsQueue.size();
        if(size == 0)
            return new byte[]{0x00, 0x00, 0x00, 0x00};
        int bufferSize = 256;
        //Calculate message length from toSendList
        int messageLength = 6;
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
        System.arraycopy(bb.array(), 0, byteArray, 0, messageLength);

        return byteArray;
    }

    public boolean isEmpty() {
        return instructionsQueue.isEmpty();
    }
}
