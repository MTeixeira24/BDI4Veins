package jasonveins.omnet.voting.Statistics;

public abstract class IStatistics {


    int iteration = -1;

    public void setSimParams(int iteration){
        this.iteration = iteration;
    }

    public void dumpCSV(){
        return;
    }

    public int getIteration(){
        return iteration;
    }
}
