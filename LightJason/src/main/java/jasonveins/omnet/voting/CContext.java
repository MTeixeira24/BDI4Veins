package jasonveins.omnet.voting;

import java.util.*;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.CopyOnWriteArrayList;

public class CContext {
    private List<Integer> m_candidates;
    private int m_voteType;
    private ConcurrentLinkedQueue<List<Integer>> m_votes;
    private int m_voterCount;
    private Map<String, Number> m_contextArguments;
    private boolean m_isIterative;

    public CContext(List<Integer> p_candidates, int p_voteType, int p_voterCount, boolean p_isIterative){
        m_candidates = p_candidates;
        m_voteType = p_voteType;
        m_votes = new ConcurrentLinkedQueue<>();
        m_voterCount = p_voterCount;
        m_contextArguments = new HashMap<>();
        m_isIterative = p_isIterative;
    }

    public boolean isIterative(){return m_isIterative;}

    public List<Integer> getCandidates() {
        return m_candidates;
    }

    public void setCandidates(List<Integer> m_candidates) {
        this.m_candidates = m_candidates;
    }

    public int getVoteType() {
        return m_voteType;
    }

    public void setVoteType(int m_voteType) {
        this.m_voteType = m_voteType;
    }

    public List<List<Integer>> getVotes() {
        return new ArrayList<>(m_votes);
    }

    public synchronized boolean pushVotes(List<Integer> p_votes){
        m_votes.offer(p_votes);
        return this.m_voterCount == m_votes.size();
    }

    public boolean allVotesSubmitted(){
        return this.m_voterCount == m_votes.size();
    }

    public void debug(){
        System.out.println(m_votes.size()+"/"+m_voterCount);
    }

    public void increaseVoterCount(){
        m_voterCount++;
    }

    public void addContextArgument(String key, Number value){
        m_contextArguments.put(key, value);
    }

    public Number getContextArgument(String key){
        return m_contextArguments.get(key);
    }

    public ArrayList<Double> getContextSequence(){
        ArrayList<Double> context = new ArrayList<>();
        context.add((double) this.m_voteType);
        for(Map.Entry<String, Number> c : m_contextArguments.entrySet()){
            context.add(c.getValue().doubleValue());
        }
        return context;
    }

    public void setVoterCount(int x){
        m_voterCount = x;
    }

    public int getVoterCount(){return m_voterCount;}

    public boolean iterationOver() {
        return m_candidates.size() <= 3;
    }
}
