package jasonveins.omnet.voting.rule;

import jasonveins.omnet.voting.CUtilityPair;

import java.util.List;
import java.util.Map;

public interface IRule {

    int getResult();

    int getResult(List<List<Integer>> votes);

    List<Integer> getVote(List<CUtilityPair> p_utilities);

}
