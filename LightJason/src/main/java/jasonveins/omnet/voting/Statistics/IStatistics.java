package jasonveins.omnet.voting.Statistics;

public abstract class IStatistics {

    int platoonSize = -1;
    String type = null;
    String rule = null;
    int iteration = -1;

    public void setSimParams(int platoonSize, int iteration, String rule, String type){
        this.platoonSize = platoonSize;
        this.rule = rule;
        this.type = type;
        this.iteration = iteration;
    }

    public void dumpCSV(){
        return;
    }

    public int getIteration(){
        return iteration;
    }
}
