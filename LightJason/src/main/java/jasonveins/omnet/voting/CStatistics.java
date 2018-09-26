package jasonveins.omnet.voting;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;

public class CStatistics {
    private int initPlatoonSpeed;
    private int finalPlatoonSpeed;
    private ConcurrentHashMap<Integer, ArrayList<Double>> initial_final_utilities;
    private boolean rejected;

    public CStatistics(){
        initial_final_utilities = new ConcurrentHashMap<>();
        rejected = false;
    }

    public void dump(){
        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter("StableApproval5.txt", true));
            bw.write("Initial platoon speed: " + initPlatoonSpeed + " Final platoon speed: " + finalPlatoonSpeed + "\n");

            if(rejected)
                bw.write("The joiner was rejected\n");
            else{
                bw.write("The joiner was not rejected\n");
                for(Map.Entry<Integer, ArrayList<Double>> e : initial_final_utilities.entrySet()){
                    bw.write(e.getKey() + " old util: " + e.getValue().get(0) + " new util: " + e.getValue().get(1)+ "\n");
                }
            }
            bw.write("#######################################################################\n");
            bw.close();
            System.out.println("Done");

        } catch (IOException e) {

            e.printStackTrace();

        }
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

    public void setInitPlatoonSpeed(int initPlatoonSpeed) {
        this.initPlatoonSpeed = initPlatoonSpeed;
    }

    public void setFinalPlatoonSpeed(int finalPlatoonSpeed) {
        this.finalPlatoonSpeed = finalPlatoonSpeed;
    }

    public void setInitAndFinalUtil(int id, double init, double fin){
        ArrayList<Double> utils = new ArrayList<>(2);
        utils.add(init);
        utils.add(fin);
        initial_final_utilities.put(id, utils);
    }

    public void setRejected(boolean val){
        rejected = val;
    }
}
