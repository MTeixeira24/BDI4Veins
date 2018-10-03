package jasonveins.omnet.voting.rule;

import jasonveins.omnet.voting.CUtilityPair;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class CBorda implements IRule {



    public CBorda() {
    }

    /**
     * Sums all votes and returns the index of the winner.
     * @param votes List of all vote vectors that have been submitted
     * @return Index of the candidate with majority votes. -1 if a tie occurs.
     */
    public int getResult(List<List<Integer>> votes, List<Integer> candidates){
        scoreVector.clear();
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
                m_tiedIndexes.add(i);
            }else if(score > highestScore){
                highestScore = score;
                winnerIndex = i;
                ties = false;
                m_tiedIndexes.clear();
            }
            scoreVector.add(score);
        }
        if(ties) return -1;
        return winnerIndex;
    }

    /**
     * Generates a vote vector based on the received utilities
     * @param p_utilities A list containing mapping of the index to its utility, ordered bottom to top
     * @return An integer list with the votes according to the voting rule
     */
    public List<Integer> getVote(List<CUtilityPair> p_utilities){
        Collections.sort(p_utilities);
        ArrayList<Integer> votes = new ArrayList<>(Collections.nCopies(p_utilities.size(), 0));
        int preferences = 0;
        for (CUtilityPair up : p_utilities) {
            votes.set(up.getIndex(), preferences++);
        }
        return votes;
    }

    /**
     * Generates a vote intended to brake a tie.
     *
     * @param p_utilities A list containing mapping of the index to its utility, ordered bottom to top
     * @return A modified integer list with the votes according to the voting rule
     */
    @Override
    public List<Integer> getTieBreakerVote(List<CUtilityPair> p_utilities, List<Integer> p_ties) {
        return getVote(p_utilities);
    }

    @Override
    public List<Integer> getTiedIndexes(){
        return m_tiedIndexes;
    }

    @Override
    public List<Integer> reduceCandidates(List<List<Integer>> votes, List<Integer> candidates){
        if(candidates.size() <= 2)
            return null;
        else{
            int newSize = candidates.size()/2;
            ArrayList<Integer> redCandidates = new ArrayList<>(newSize);
            this.getResult(votes, candidates);
            for(int i = 0; i < newSize; i++){
                int highestIndex = scoreVector.indexOf(Collections.max(scoreVector));
                redCandidates.add(candidates.get(highestIndex));
                scoreVector.set(highestIndex, -1);
            }
            return redCandidates;
        }
    }

    @Override
    public int getExpectedVoteSize(int size){
        return size;
    }
}
