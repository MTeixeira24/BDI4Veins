package jasonveins.omnet.voting.rule;

import jasonveins.omnet.voting.CUtilityPair;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class CApproval implements IRule{

    @Override
    public int getResult(List<List<Integer>> votes, List<Integer> candidates) {
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

    public List<Integer> getApprovalSum(List<List<Integer>> votes, List<Integer> candidates){
        getResult(votes, candidates);
        return scoreVector;
    }

    @Override
    public List<Integer> getVote(List<CUtilityPair> p_utilities) {
        Collections.sort(p_utilities);
        ArrayList<Integer> votes = new ArrayList<>(Collections.nCopies(p_utilities.size(), 1));
        for (CUtilityPair up : p_utilities) {
            if(up.getUtility() < 0.8)
                votes.set(up.getIndex(), 0);
            else
                break; //The list is sorted so just break
        }
        votes.set(p_utilities.get(0).getIndex(), 0); //The least preferred option is always zero to minimize ties
        return votes;
    }

    /**
     * Generates a vote intended to break a tie.
     *
     * @param p_utilities A list containing mapping of the index to its utility, ordered bottom to top
     * @return A modified integer list with the votes according to the voting rule
     */
    @Override
    public List<Integer> getTieBreakerVote(List<CUtilityPair> p_utilities, List<Integer> p_ties) {
        Collections.sort(p_utilities);
        //Plurality vote style tie braking. Of the tied candidates choose the one that offers the
        //chair higher utility
        ArrayList<Integer> votes = new ArrayList<>(Collections.nCopies(p_utilities.size(), 0));
        int highestIndex = -1;
        double highestUtility = -1;
        for(int t : p_ties){
            if(p_utilities.get(t).getUtility() > highestUtility){
                highestIndex = t;
                highestUtility = p_utilities.get(t).getUtility();
            }
        }
        votes.set(highestIndex, 1);
        return votes;
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

    @Override
    public List<Integer> getResultVector(List<List<Integer>> votes, List<Integer> candidates, int scoreThreshold){
        getResult(votes, candidates);
        return scoreVector;
    }
}
