package jasonveins.omnet.statistics;

public class CMarketStatistics extends IStatistics {

    private int platoonSize;
    private String type;
    private String auctionModule;

    public CMarketStatistics(){
    }

    public void dumpCSV(){

    }

    public void setSimParams(int platoonSize, int iteration, String type, String auctionModule){
        this.platoonSize = platoonSize;
        this.type = type;
        this.iteration = iteration;
        this.auctionModule = auctionModule;
    }
}
