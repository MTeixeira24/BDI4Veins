package jasonveins.omnet.agent.voting;

import jasonveins.omnet.agent.utilityFunctions.IUtilityFunction;
import jasonveins.omnet.managers.constants.VoteConstants;
import jasonveins.omnet.voting.CContext;
import jasonveins.omnet.voting.CUtilityPair;
import jasonveins.omnet.voting.rule.*;

import java.util.ArrayList;
import java.util.List;

public class SingleRule {

    private IRule votingRule;

    SingleRule(String ruleName){
        switch (ruleName){
            case "Borda":{
                votingRule = new CBorda();
                break;
            }
            case "Approval":{
                votingRule = new CApproval();
                break;
            }
            case "Plurality":{
                votingRule = new CPlurality();
                break;
            }
            case "Copeland":{
                votingRule = new CCopeland();
                break;
            }
            default:{
                throw new RuntimeException("CVoterAgent: Unknown voteRule!");
            }
        }
    }

    int expectedVoteSize(int candidateCount){return votingRule.getExpectedVoteSize(candidateCount);}

    List<Integer> generateVoteVector(List<Integer> p_candidates, IUtilityFunction ufunc){
        List<CUtilityPair> utils = new ArrayList<>(p_candidates.size());
        for(int i = 0; i < p_candidates.size(); i++) {
            double util = ufunc.computeUtilitySpeed(p_candidates.get(i));
            utils.add(new CUtilityPair(i, util));
        }
        return votingRule.getVote(utils);
    }

    int determineWinner(CContext p_context){
        System.out.println("FINAL ITERATION");
        int winnerIndex = votingRule.getResult(p_context.getVotes(), p_context.getCandidates());
        if(winnerIndex == -1)
            return  -1;
        int winner = p_context.getCandidates().get(winnerIndex);
        System.out.println("Index that won is: " + winner);
        return winner;
    }

    List<Integer> tieBrakeVote(CContext p_context, IUtilityFunction ufunc){
        ArrayList<Integer> l_candidates = new ArrayList<>(p_context.getCandidates().size());
        l_candidates.addAll(p_context.getCandidates());
        List<CUtilityPair> utils = new ArrayList<>(l_candidates.size());
        for(int i = 0; i < l_candidates.size(); i++) {
            double util = ufunc.computeUtilitySpeed(l_candidates.get(i));
            utils.add(new CUtilityPair(i, util));
        }
        return votingRule.getTieBreakerVote(utils, votingRule.getTiedIndexes());
    }

    List<Integer> reduceCandidates(CContext p_context){
        return votingRule.reduceCandidates(p_context.getVotes(),p_context.getCandidates());
    }
}
