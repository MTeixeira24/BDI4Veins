package jasonveins.omnet.decision;

import java.nio.ByteBuffer;
import java.util.ArrayList;

public class InstructionModel {
    private int size;
    private int agentId;
    private int agentAction;
    private String agentBelief;
    private ArrayList<ByteData> values = new ArrayList<>();

    public InstructionModel(int _agentId, int _agentAction) {
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
        values.add(new ByteData(type, ByteBuffer.allocate(1).putChar(_value).array()));
        size += 4; //two bytes for valueType, remainder for data
    }
    public void pushShort(short _value){
        short type = 3;
        values.add(new ByteData(type, ByteBuffer.allocate(2).putShort(_value).array()));
        size += 4;
    }
    public void pushInt(int _value){
        short type = 4;
        values.add(new ByteData(type, ByteBuffer.allocate(4).putInt(_value).array()));
        size += 6;
    }
    public void pushLong(long _value){
        short type = 5;
        values.add(new ByteData(type, ByteBuffer.allocate(8).putLong(_value).array()));
        size += 10;
    }
    public void pushFloat(float _value){
        short type = 6;
        values.add(new ByteData(type, ByteBuffer.allocate(4).putFloat(_value).array()));
        size += 6;
    }
    public void pushDouble(double _value){
        short type = 7;
        values.add(new ByteData(type, ByteBuffer.allocate(8).putDouble(_value).array()));
        size += 10;
    }

    public void pushRawBytes(short _type, byte[] _value){
        values.add(new ByteData(_type, _value));
    }

    public void setBelief(String _belief){
        agentBelief = _belief;
    }

    public byte[] toByteArray(){
        ByteBuffer bb = ByteBuffer.allocate(this.size);
        bb.putInt(this.size);
        bb.putInt(getAgentId());
        bb.putInt(getAgentAction());
        for(ByteData op : values){
            bb.putShort(op.valueType());
            bb.put(op.value());
        }
        return bb.array();
    }
}
