package jasonveins.omnet.voting.rule;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public class CMinisum extends CApproval {
    @Override
    public List<Integer> getResultVector(List<List<Integer>> votes, List<Integer> candidates, int size){
        getResult(votes, candidates);
        /*
        ArrayList<Integer> determinationResults = new ArrayList<>(scoreVector);
        //Find the size largest scores
        Integer[] copy = determinationResults.toArray(new Integer[0]);
        Arrays.sort(copy);
        Integer[] honey = Arrays.copyOfRange(copy,copy.length - size, copy.length);
        int[] result = new int[size];
        int resultPos = 0;
        for(int i = 0; i < determinationResults.size(); i++) {
            int onTrial = determinationResults.get(i);
            int index = Arrays.binarySearch(honey,onTrial);
            if(index < 0) continue;
            result[resultPos++] = i;
        }
        return result;
        */
        return scoreVector;
    }
}
