package jasonveins.omnet.statistics;

import java.io.File;
import java.io.FileWriter;
import java.util.*;

import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;

public class CJoinStatistics extends IStatistics {

    private ConcurrentHashMap<Integer, ArrayList<Double>> initial_final_utilities;
    private ConcurrentHashMap<Integer, ArrayList<Integer>> platoonIds;
    private boolean rejected;
    private int platoonSize = -1;
    private String type = null;
    private String rule = null;
    private String committeeRule = null;

    public CJoinStatistics(){
        initial_final_utilities = new ConcurrentHashMap<>();
        platoonIds = new ConcurrentHashMap<>();
        rejected = false;
    }

    public void dumpCSV(){

        try {
            FileWriter out;
            File f = new File("testResults/JoinResults.csv");
            if(!f.exists() || f.isDirectory()) {
                out = new FileWriter(f);
                //out.write("Scenario,Rule,CommitteeRule,PlatoonSize,Iteration,Rejected,Agent,InitialUtility,FinalUtility,Difference,HammingDistance\n");
                out.write("Scenario,Rule,CommitteeRule,PlatoonSize,Iteration,Rejected,Agent,Utility1,HammingDistance1,Utility2,HammingDistance2,Utility3,HammingDistance3,Platoon1,Platoon2\n");
            }else{
                out = new FileWriter(f, true);
            }
            String head = type+","+rule+","+committeeRule+","+platoonSize+","+iteration+',';
            if(rejected){
                out.write(head+"1,,,,,,,\n");
            }else{
                head = head + "0,";
                Iterator<Map.Entry<Integer, ArrayList<Double>>> it = initial_final_utilities.entrySet().iterator();
                for(int i = 0; i < initial_final_utilities.entrySet().size(); i++){
                    Map.Entry<Integer, ArrayList<Double>> e = it.next();
                    if(e.getValue().size() < 6)
                        for(int n = e.getValue().size(); n < 6; n++) e.getValue().add((double)0);
                    out.write(head+e.getKey());
                    for(int j = 0; j < 6; j += 2){
                        out.write("," + e.getValue().get(j) + "," + e.getValue().get(j+1));
                    }
                    platoonIds.computeIfAbsent(i, k -> new ArrayList<>(Collections.nCopies(2, -1)));
                    for (Integer pids : platoonIds.get(i)) {
                        out.write("," + pids);
                    }
                    out.write("\n");
                }
            }
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void storeUtil(int id, double util, double hamming){
        ArrayList<Double> utils = initial_final_utilities.get(id);
        if(utils == null){
            utils = new ArrayList<>(6);
        }
        //utils.add(init);
        utils.add(util);
        utils.add(hamming);
        initial_final_utilities.put(id, utils);
    }

    public void storePlatoon(int id, int platoonId){
        platoonIds.computeIfAbsent(id, k -> new ArrayList<>(Collections.nCopies(2, -1)));
        platoonIds.get(id).set(platoonIds.get(id).indexOf(-1), platoonId);
    }

    public void setRejected(boolean val){
        rejected = val;
    }

    public void setSimParams(int platoonSize, int iteration, String rule, String type, String committeeRule){
        this.platoonSize = platoonSize;
        this.rule = rule;
        this.type = type;
        this.committeeRule = committeeRule;
        this.iteration = iteration;
    }
}
