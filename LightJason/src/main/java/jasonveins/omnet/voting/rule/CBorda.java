package jasonveins.omnet.voting.rule;

import cern.colt.bitvector.BitVector;

import java.util.ArrayList;
import java.util.List;

public class CBorda {

    private List<Integer> m_candidates;
    private List<BitVector> m_bitVotes;
    private ArrayList<Integer> results;

    public CBorda(List<Integer> p_candidates, List<BitVector> p_bitVotes) {
        if(p_candidates.size() == p_bitVotes.get(0).size()){
            m_candidates = p_candidates;
            m_bitVotes = p_bitVotes;
            results = new ArrayList<>();
        }
    }

    public int getResult(){
        int highestIndex = -1;
        int highestScore = -1;
        for(int i = 0; i < m_candidates.size(); i++){
            int score = 0;
            for(int j = 0; j < m_bitVotes.size(); j++){
                score += m_bitVotes.get(j).get(i) ? 1 : 0;
            }
            if(score > highestScore){
                highestScore = score;
                highestIndex = i;
            }
        }
        //TODO: Tiebraker;
        return highestIndex;
    }
}
