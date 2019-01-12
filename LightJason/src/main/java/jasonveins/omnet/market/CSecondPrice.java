package jasonveins.omnet.market;

public class CSecondPrice extends IAuctionModule {

    private int highestAmount = -1;
    private int secondHighestAmount = -2;

    public CSecondPrice(int agentId) {
        super(agentId);
    }

    @Override
    public void addBid(int agentId, int amount){
        bidSet.put(agentId, amount);
        if(amount > highestAmount){
            winner = agentId;
            secondHighestAmount = highestAmount;
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

    @Override
    public int getDuePayment(){
        return secondHighestAmount;
    }

}
