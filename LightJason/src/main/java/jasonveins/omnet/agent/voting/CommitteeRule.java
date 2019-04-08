package jasonveins.omnet.agent.voting;

import jasonveins.omnet.environment.dijkstra.Dijkstra;
import jasonveins.omnet.environment.dijkstra.Graph;
import jasonveins.omnet.environment.dijkstra.Vertex;
import jasonveins.omnet.managers.constants.VoteConstants;
import jasonveins.omnet.voting.CContext;
import jasonveins.omnet.voting.CUtilityPair;
import jasonveins.omnet.voting.rule.CApproval;
import jasonveins.omnet.voting.rule.CMinimax;
import jasonveins.omnet.voting.rule.CMinisum;

import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
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

    List<Integer> generateVoteVector(List<Integer> p_candidates, List<Integer> p_context, List<Integer> nodes){
        ArrayList<Integer> l_candidates = new ArrayList<>(p_candidates.size());
        short l_context = p_context.get(0).shortValue();
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

    List<Integer> determineWinner(CContext p_context, CVoteParameters p_voteParams){
        System.out.println("COMMITTEE WINNER DETERMINATION");
        LinkedList<Integer> scoreVector = new LinkedList<>();
        int size = p_context.getVoteType() == VoteConstants.CONTEXT_PATH ? -1 : p_context.getCandidates().size() / 2;
        ArrayList<Integer> results = new ArrayList<>(getResultVector(p_context, size));
        //InstructionModel iOb = new InstructionModel(this.id, VoteConstants.SEND_VOTE_RESULTS);
        //iOb.pushInt(-1);
        if(p_context.getVoteType() == VoteConstants.CONTEXT_PATH){
            //Store the path to take
            p_voteParams.setCurrentPath(p_voteParams.getRoute().updateAndComputeShortestPath(results));
            //Convert to a score list
            ArrayList<Integer> pathIds = new ArrayList<>();
            for (Vertex aPath : p_voteParams.getCurrentPath()) {
                pathIds.add(Integer.parseInt(aPath.getId()));
            }
            for(int i = 0; i < p_context.getCandidates().size(); i++){
                if(pathIds.contains(p_context.getCandidates().get(i)))
                    scoreVector.add(1);
                else
                    scoreVector.add(0);
            }
            //iOb.pushIntArray(scoreVector);

            /*final ITrigger l_trigger = CTrigger.from(
                    ITrigger.EType.ADDGOAL,
                    CLiteral.from(
                            "handle/results/committee",
                            CRawTerm.from(scoreVector)
                    )

            );
            this.trigger( l_trigger );*/
        }else{
            LinkedList<Integer> p1 = new LinkedList<>();
            for (int i = 0; i < results.size(); i++) {
                if(results.get(i) == 1) scoreVector.add(i);
            }
            //iOb.pushIntArray(p1);
        }
        return scoreVector;
    }


    List<Integer> getResultVector(CContext context, int size){
        return  votingRule.getResultVector(context.getVotes(), context.getCandidates(), size);
    }
}
