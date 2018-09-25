package jasonveins.omnet.voting;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class CStatistics {
    public int initPlatoonSpeed;
    public int finalPlatoonSpeed;
    public HashMap<Integer, ArrayList<Double>> initial_final_utilities;
    public boolean rejected;

    public CStatistics(){
        initial_final_utilities = new HashMap<>();
        rejected = false;
    }

    public void dump(){
        System.out.println("Initial platoon speed: " + initPlatoonSpeed + " Final platoon speed: " + finalPlatoonSpeed);
        if(rejected)
            System.out.println("The joiner was rejected");
        else{
            System.out.println("The joiner was not rejected");
            for(Map.Entry<Integer, ArrayList<Double>> e : initial_final_utilities.entrySet()){
                System.out.println("Agent " + e.getKey() + " started with util: " + e.getValue().get(0) + " and ended with: " + e.getValue().get(1));
            }
        }
    }
}
