package jasonveins.omnet.market;

import java.util.List;

public class CFirstPrice extends IAuctionModule {

    private int highestAmount = -1;

    public CFirstPrice(int agentId) {
        super(agentId);
    }

    @Override
    public void addBid(int agentId, int amount){
        bidSet.put(agentId, amount);
        if(amount > highestAmount){
            winner = agentId;
            highestAmount = amount;
        }
    }

    @Override
    public boolean endOfAuction(){
        if((getAuctionIteration() == 4) || (withdrawCount >= getBidders().size() - 1))
                hasEnded = true;
        return hasEnded;
    }

    @Override
    public int determineWinner() {
        return winner;
    }

}
