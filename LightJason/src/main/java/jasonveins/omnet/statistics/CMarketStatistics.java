package jasonveins.omnet.statistics;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

public class CMarketStatistics extends IStatistics {

    private int platoonSize;
    private String type;
    private String auctionModule;
    private ConcurrentHashMap<Integer, ArrayList<Double>> utilities;
    private ConcurrentHashMap<Integer, ArrayList<Double>> hamming;
    private ConcurrentHashMap<Integer, ArrayList<Double>> hammingUtilities;
    private ConcurrentHashMap<Integer, ArrayList<Integer>> platoonIds;
    private ConcurrentLinkedQueue<Budget> transactions;


    public CMarketStatistics(){
        utilities = new ConcurrentHashMap<>();
        hamming = new ConcurrentHashMap<>();
        hammingUtilities = new ConcurrentHashMap<>();
        platoonIds = new ConcurrentHashMap<>();
        transactions = new ConcurrentLinkedQueue<>();
    }

    public void dumpCSV(){
        try {
            FileWriter out;
            File f = new File("testResults/MarketResults.csv");
            if(!f.exists() || f.isDirectory()) {
                out = new FileWriter(f);
                //out.write("Scenario,Rule,CommitteeRule,PlatoonSize,Iteration,Rejected,Agent,InitialUtility,FinalUtility,Difference,HammingDistance\n");
                out.write("Scenario,AuctionType,PlatoonSize,Iteration,Agent,Utility1,HammingDistance1," +
                        "RouteUtil1,Utility2,HammingDistance2,RouteUtil2,Endowment,Flow1,Flow2,Flow3,Flow4,Flow5\n");
            }else{
                out = new FileWriter(f, true);
            }
            String head = type+","+auctionModule+","+platoonSize+","+iteration+',';
            Iterator<Map.Entry<Integer, ArrayList<Double>>> it = utilities.entrySet().iterator();
            HashMap<Integer, ArrayList<Budget>> transactions =  mapTransactions();
            for(int i = 0; i < utilities.entrySet().size(); i++){
                Map.Entry<Integer, ArrayList<Double>> e = it.next();
                int id = e.getKey();
                try{
                    if(e.getValue().size() < 2){
                        for(int n = e.getValue().size(); n <= 2; n++) e.getValue().add(0.0);
                        for(int n = hamming.get(id).size(); n <= 2; n++) hamming.get(id).add(0.0);
                        for(int n = hammingUtilities.get(id).size(); n <= 2; n++) hammingUtilities.get(id).add(0.0);
                    }
                }catch (NullPointerException t){
                    t.printStackTrace();
                }
                out.write(head+e.getKey());
                for(int j = 0; j < 2; j++){
                    out.write("," + e.getValue().get(j) +","+hamming.get(id).get(j)+","+hammingUtilities.get(id).get(j));
                }
                out.write(","+transactions.get(id).get(0).budget);
                for (int j = 1; j < transactions.get(id).size(); j++) {
                    out.write(","+transactions.get(id).get(j).flow);
                }
                out.write("\n");
            }
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    public void addTransaction(int p_budget, int p_flow, int p_agentId, int p_movementNumber){
        transactions.add(new Budget(p_budget, p_flow, p_agentId, p_movementNumber));
    }

    private HashMap<Integer, ArrayList<Budget>> mapTransactions(){
        HashMap<Integer, ArrayList<Budget>> transactionsMap = new HashMap<>();
        while(!transactions.isEmpty()){
            Budget budget = transactions.poll();
            transactionsMap.computeIfAbsent(budget.agentId, k -> new ArrayList<>(6));
            transactionsMap.get(budget.agentId).add(budget);
        }
        for(int agent : transactionsMap.keySet()){
            int remainder = 6 - transactionsMap.get(agent).size();
            for(; remainder > 0; remainder--){
                transactionsMap.get(agent).add(new Budget(0, 0, agent, -1));
            }
        }
        return transactionsMap;
    }

    public void storeUtil(int id, double util){
        ArrayList<Double> utils = utilities.get(id);
        if(utils == null){
            utils = new ArrayList<>(6);
        }
        utils.add(util);
        utilities.put(id, utils);
    }
    public void storeHamming(int id, double hamming){
        ArrayList<Double> utils = this.hamming.get(id);
        if(utils == null){
            utils = new ArrayList<>(6);
            this.hamming.putIfAbsent(id, utils);
        }
        this.hamming.get(id).add(hamming);
    }
    public void storeHammingUtil(int id, double hamming){
        ArrayList<Double> utils = hammingUtilities.get(id);
        if(utils == null){
            utils = new ArrayList<>(6);
            hammingUtilities.putIfAbsent(id, utils);
        }
        hammingUtilities.get(id).add(hamming);
    }

    public void storePlatoon(int id, int platoonId){
        platoonIds.computeIfAbsent(id, k -> new ArrayList<>(Collections.nCopies(2, -1)));
        platoonIds.get(id).set(platoonIds.get(id).indexOf(-1), platoonId);
    }

    public void setSimParams(int platoonSize, int iteration, String type, String auctionModule){
        this.platoonSize = platoonSize;
        this.type = type;
        this.iteration = iteration;
        this.auctionModule = auctionModule;
    }

    public class Budget{
        int budget;
        int flow;
        int agentId;
        int movementNumber;

        public Budget(int p_budget, int p_flow, int p_agentId, int p_movementNumber){
            budget = p_budget;
            flow = p_flow;
            agentId = p_agentId;
            movementNumber = p_movementNumber;
        }
    }
}
