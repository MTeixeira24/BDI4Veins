package jasonveins.omnet.voting.Statistics;

public abstract class IStatistics {

    int platoonSize = -1;
    String type = null;
    String rule = null;
    int iteration = -1;
    String committeeRule = null;

    public void setSimParams(int platoonSize, int iteration, String rule, String type, String committeeRule){
        this.platoonSize = platoonSize;
        this.rule = rule;
        this.type = type;
        this.iteration = iteration;
        this.committeeRule = committeeRule;
    }

    public void dumpCSV(){
        return;
    }

    public int getIteration(){
        return iteration;
    }
}
