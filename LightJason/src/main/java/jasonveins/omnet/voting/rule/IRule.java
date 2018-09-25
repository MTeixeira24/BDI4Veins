package jasonveins.omnet.voting.rule;

import jasonveins.omnet.voting.CUtilityPair;

import java.util.ArrayList;
import java.util.List;

public interface IRule {

    List<Integer> m_tiedIndexes = new ArrayList<>();

    /**
     * Sums all votes and returns the index of the winner.
     * @param votes List of all vote vectors that have been submitted
     * @return Index of the candidate with majority votes. -1 if a tie occurs.
     */
    int getResult(List<List<Integer>> votes, List<Integer> candidates);

    /**
     * Generates a vote vector based on the received utilities
     * @param p_utilities A list containing mapping of the index to its utility, ordered bottom to top
     * @return An integer list with the votes according to the voting rule
     */
    List<Integer> getVote(List<CUtilityPair> p_utilities);

    /**
     * Generates a vote intended to brake a tie.
     * @param p_utilities A list containing mapping of the index to its utility, ordered bottom to top
     * @return A modified integer list with the votes according to the voting rule
     */
    List<Integer> getTieBreakerVote(List<CUtilityPair> p_utilities, List<Integer> p_ties);

    List<Integer> getTiedIndexes();

}
