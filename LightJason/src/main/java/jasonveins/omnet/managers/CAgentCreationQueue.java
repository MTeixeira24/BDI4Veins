package jasonveins.omnet.managers;

import java.util.LinkedList;
import java.util.concurrent.ConcurrentLinkedQueue;

public class CAgentCreationQueue {

    private ConcurrentLinkedQueue<CAgentData> agentsToInsert;

    public CAgentCreationQueue(){
        agentsToInsert = new ConcurrentLinkedQueue<>();
    }

    public void pushAgent(int p_id, String p_aslFile, String p_vType){
        agentsToInsert.offer(new CAgentData(p_id, p_aslFile, p_vType));
    }

    public CAgentData popAgent(){
        return agentsToInsert.poll();
    }

    public CAgentData peekAgent(){
        return agentsToInsert.peek();
    }

    public int size(){
        return agentsToInsert.size();
    }

    public class CAgentData{
        private int id;
        private String aslFile;
        private String vType;

        public CAgentData(int p_id, String p_aslFile, String p_vType){
            id = p_id;
            aslFile = p_aslFile;
            vType = p_vType;
        }

        public int getId() {
            return id;
        }

        public String getAslFile() {
            return aslFile;
        }

        public String getvType() {
            return vType;
        }
    }
}
