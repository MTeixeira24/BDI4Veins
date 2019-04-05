package jasonveins.omnet.agent.voting;

import jasonveins.omnet.managers.constants.VoteConstants;
import jasonveins.omnet.voting.CContext;
import jasonveins.omnet.voting.CUtilityPair;
import jasonveins.omnet.voting.rule.CApproval;
import jasonveins.omnet.voting.rule.CMinimax;
import jasonveins.omnet.voting.rule.CMinisum;

import java.util.ArrayList;
import java.util.List;

public class CommitteeRule {
    private CApproval votingRule;

    CommitteeRule(String ruleName){
        switch (ruleName){
            case "Minisum":{
                votingRule = new CMinisum();
                break;
            }
            case "Minimax":{
                votingRule = new CMinimax();
                break;
            }
            default:{
                throw new RuntimeException("CVoterAgent: Unknown committeeRule!");
            }
        }
    }

    int expectedVoteSize(int candidateCount){return votingRule.getExpectedVoteSize(candidateCount);}

    List<Integer> generateVoteVector(List<Integer> p_candidates, List<Double> p_context, List<Integer> nodes){
        ArrayList<Integer> l_candidates = new ArrayList<>(p_candidates.size());
        short l_context = (short)p_context.get(0).doubleValue();
        if (l_context == VoteConstants.CONTEXT_REGROUP){
            p_context.subList(1,p_context.size()).forEach( j -> l_candidates.add(j.intValue()));
        }else{
            l_candidates.addAll(p_candidates);
        }
        List<CUtilityPair> utils = new ArrayList<>(l_candidates.size());
        for(int i = 0; i < l_candidates.size(); i++){
            if(nodes.contains(l_candidates.get(i)))
                utils.add(new CUtilityPair(i, 1));
            else
                utils.add(new CUtilityPair(i, 0));
        }
        return votingRule.getVote(utils);
    }

    List<Integer> getApprovalSum(CContext context){return votingRule.getApprovalSum(context.getVotes(), context.getCandidates());}

    List<Integer> getResultVector(CContext context, int size){
        return  votingRule.getResultVector(context.getVotes(), context.getCandidates(), size);
    }
}
