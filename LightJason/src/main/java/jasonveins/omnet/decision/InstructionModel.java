package jasonveins.omnet.decision;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

import static jasonveins.omnet.managers.Constants.*;

public class InstructionModel {
    private int size;
    private int agentId;
    private int agentAction;
    private String agentBelief;
    private ArrayList<ByteData> values = new ArrayList<>();



    public InstructionModel(int _agentId, int _agentAction) {
        agentId = _agentId;
        agentAction = _agentAction;
        size = 12; //size + agentid + agentaction
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
        short type = VALUE_CHAR;
        values.add(new ByteData(type, ByteBuffer.allocate(1).putChar(_value).array()));
        size += 4; //two bytes for valueType, remainder for data
    }
    public void pushShort(short _value){
        short type = VALUE_SHORT;
        values.add(new ByteData(type, ByteBuffer.allocate(2).putShort(_value).array()));
        size += 4;
    }
    public void pushInt(int _value){
        short type = VALUE_INT;
        values.add(new ByteData(type, ByteBuffer.allocate(4).putInt(_value).array()));
        size += 6;
    }
    public void pushLong(long _value){
        short type = VALUE_LONG;
        values.add(new ByteData(type, ByteBuffer.allocate(8).putLong(_value).array()));
        size += 10;
    }
    public void pushFloat(float _value){
        short type = VALUE_FLOAT;
        values.add(new ByteData(type, ByteBuffer.allocate(4).putFloat(_value).array()));
        size += 6;
    }
    public void pushDouble(double _value){
        short type = VALUE_DOUBLE;
        values.add(new ByteData(type, ByteBuffer.allocate(8).putDouble(_value).array()));
        size += 10;
    }
    public void pushIntArray(List<Integer> p_list){
        ByteBuffer bb = ByteBuffer.allocate(6 + 4*p_list.size());
        //Type of elements in array
        bb.putShort(VALUE_INT);
        //Content size
        bb.putInt(p_list.size());
        //Contents
        for(int e : p_list){
            bb.putInt(e);
        }
        ByteData arrayData = new ByteData(VALUE_ARRAY, bb.array());
        size += 8 + (4*p_list.size()); //2 bytes for array identifier, 2 bytes for array data type, 4 bytes for array length, remainder is data
        values.add(arrayData);
    }
    public void pushDoubleArray(ArrayList<Double> p_list){
        ByteBuffer bb = ByteBuffer.allocate(6 + 8*p_list.size());
        //Type of elements in array
        bb.putShort(VALUE_DOUBLE);
        //Content size
        bb.putInt(p_list.size());
        //Contents
        for(double e : p_list){
            bb.putDouble(e);
        }
        ByteData arrayData = new ByteData(VALUE_ARRAY, bb.array());
        size += 8 + (8*p_list.size()); //2 bytes for array identifier, 2 bytes for array data type, 4 bytes for array length, remainder is data
        values.add(arrayData);
    }

    /*public void pushRawBytes(short _type, byte[] _value){
        values.add(new ByteData(_type, _value));
    }*/

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
