package jasonveins.omnet.market;

import java.util.List;

public class CStochasticLocalSearch extends IAuctionModule {

    public CStochasticLocalSearch(int agentId) {
        super(agentId);
    }

    @Override
    public int determineWinner() {
        return 0;
    }

    @Override
    public boolean endOfAuction() {
        return false;
    }

    @Override
    public int getDuePayment() {
        return 0;
    }

}
