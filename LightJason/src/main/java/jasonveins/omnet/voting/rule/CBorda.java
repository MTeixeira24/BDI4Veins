package jasonveins.omnet.voting.rule;

import cern.colt.bitvector.BitVector;
import jasonveins.omnet.voting.CUtilityPair;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;

public class CBorda implements IRule {

    private List<Integer> m_candidates;
    private List<BitVector> m_bitVotes;
    private ArrayList<Integer> results;

    public CBorda() {

    }

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

    /**
     * Sums all votes and returns the index of the winner.
     * @param votes List of all vote vectors that have been submitted
     * @return Index of the candidate with majority votes. -1 if a tie occurs.
     */
    public int getResult(List<List<Integer>> votes){
        boolean ties = false;
        int winnerIndex = -1;
        int highestScore = -1;
        int voteVectorLength = votes.get(0).size();
        for(int i = 0; i < voteVectorLength; i++){
            int score = 0;
            for(int j = 0; j < votes.size(); j++){
                score += votes.get(j).get(i);
            }
            if(score == highestScore){
                ties = true;
            }else if(score > highestScore){
                highestScore = score;
                winnerIndex = i;
            }
        }
        if(ties) return -1;
        return winnerIndex;
    }

    public List<Integer> getVote(List<CUtilityPair> p_utilities){
        ArrayList<Integer> votes = new ArrayList<>(Collections.nCopies(p_utilities.size(), 0));
        int preferences = 0;
        for (CUtilityPair up : p_utilities) {
            votes.set(up.getIndex(), preferences++);
        }
        return votes;
    }
}
