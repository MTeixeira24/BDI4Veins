package jasonveins.omnet.agent.voting;

import jasonveins.omnet.agent.utilityFunctions.IUtilityFunction;
import jasonveins.omnet.managers.constants.VoteConstants;
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

    private List<Integer> generateVoteVector(List<Integer> p_candidates, IUtilityFunction ufunc){
        ArrayList<Integer> l_candidates = new ArrayList<>(p_candidates.size());
        l_candidates.addAll(p_candidates);
        List<CUtilityPair> utils = new ArrayList<>(l_candidates.size());
        for(int i = 0; i < l_candidates.size(); i++) {
            double util = ufunc.computeUtilitySpeed(l_candidates.get(i));
            utils.add(new CUtilityPair(i, util));
        }
        return votingRule.getVote(utils);
    }

}
