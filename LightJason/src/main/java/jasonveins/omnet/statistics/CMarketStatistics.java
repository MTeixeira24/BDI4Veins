package jasonveins.omnet.statistics;

public class CMarketStatistics extends IStatistics {

    private int platoonSize;
    private String type;

    public CMarketStatistics(){
    }

    public void dumpCSV(){

    }

    public void setSimParams(int platoonSize, int iteration, String type){
        this.platoonSize = platoonSize;
        this.type = type;
        this.iteration = iteration;
    }
}
