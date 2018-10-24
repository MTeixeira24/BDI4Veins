package jasonveins.omnet.voting.rule;


/*
Creates an adapter to call voting results from the CMinimaxApproval method
from the J-Voting implementation
 */

import cern.colt.bitvector.BitVector;
import jasonveins.lightvoting.simulation.rule.CMinimaxApproval;

import java.util.ArrayList;
import java.util.List;

public class CMinimax extends CApproval {
    @Override
    public List<Integer> getResultVector(List<List<Integer>> votes, List<Integer> candidates, int size){
        getResult(votes, candidates);
        List<String> alternatives = new ArrayList<>();
        List<BitVector> bit_votes = new ArrayList<>();
        //Convert the candidate list
        for (Integer candidate : candidates) alternatives.add(candidate.toString());
        //Convert votes to a list of BitVectors
        for (List<Integer> vote : votes) {
            BitVector bv = new BitVector(candidates.size());
            for (int j = 0; j < candidates.size(); j++) {
                boolean bit = vote.get(j) <= 0;
                bv.put(j, bit);
            }
            bit_votes.add(bv);
        }
        //Get the results
        BitVector bit_results = new CMinimaxApproval().applyRuleBV(alternatives, bit_votes, size);
        //Convert to integer list for return
        List<Integer> results = new ArrayList<>(bit_results.size());
        for(int i = 0; i < bit_results.size(); i++){
            int intValue = bit_results.get(i) ? 1 : 0;
            results.add(intValue);
        }

        return results;
    }
}
