package jasonveins.omnet.market;

import java.util.List;

public class CFirstPrice extends IAuctionModule {

    private int highestAmount = -1;

    @Override
    public void addBid(int agentId, int amount){
        bidSet.put(agentId, amount);
        if(amount > highestAmount){
            winner = agentId;
            highestAmount = amount;
        }
    }

    @Override
    public int determineWinner() {
        return winner;
    }

    @Override
    public List<Integer> determineWinnerSet() {
        return null;
    }
}
