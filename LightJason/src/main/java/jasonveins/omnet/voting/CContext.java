package jasonveins.omnet.voting;

import java.util.ArrayList;

public class CContext {
    private ArrayList<Integer> m_candidates;
    private String m_voteType;

    public CContext(ArrayList<Integer> p_candidates, String p_voteType){
        m_candidates = p_candidates;
        m_voteType = p_voteType;
    }


    public ArrayList<Integer> getCandidates() {
        return m_candidates;
    }

    public void setCandidates(ArrayList<Integer> m_candidates) {
        this.m_candidates = m_candidates;
    }

    public String getVoteType() {
        return m_voteType;
    }

    public void setVoteType(String m_voteType) {
        this.m_voteType = m_voteType;
    }
}
