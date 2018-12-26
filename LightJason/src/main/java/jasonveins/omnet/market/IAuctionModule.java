package jasonveins.omnet.market;

import jasonveins.omnet.managers.constants.MarketConstants;

import java.util.*;

public abstract class IAuctionModule {
    private int endowment;
    private int wtp;
    private LinkedList<Integer> bidders;
    private ArrayList<Integer> winnerSet;
    HashMap<Integer, Integer> bidSet;
    protected int winner;
    private double auctionId;
    private int auctionIteration;
    private Random rd;
    private int currentBid;

    public IAuctionModule(int _endowment, int _wtp, int _auctionId){
        rd = new Random();
        endowment = _endowment;
        wtp = _wtp;
        auctionId  = _auctionId;
        auctionIteration = 0;
    }

    public IAuctionModule(){
        this(100, 80, 0);
    }

    public int castBid(int roundStatus){
        int bid = 0;
        switch (roundStatus){
            case MarketConstants.AUCTION_START:{
                //Pull a random bid from a normal distribution of mean wtp/2 and variance 10
                bid = (int)Math.floor(
                        rd.nextGaussian()*10 + (wtp/2)
                );
                break;
            }
            case MarketConstants.REJECTED:
            case MarketConstants.DECOMMITTED:{
                //Increment the bid by steps of 25% the wtp, until a value equal to wpt is reached
                if(bid == wtp) return -1;
                bid = currentBid + (int)Math.floor(wtp/4);
                if(bid > wtp) bid = wtp;
                break;
            }
            case MarketConstants.CONFIRMED:{
                //If confirmed don't bid.
                return -1;
            }
        }
        if(bid > endowment) return -1;
        currentBid = bid;
        return bid;
    }
    public abstract int determineWinner();
    public abstract List<Integer> determineWinnerSet();

    /*
    Getters and setters
     */

    public int getWinner(){return winner;}
    public List<Integer> getWinnerSet(){return winnerSet;}
    public int getEndowment() {
        return endowment;
    }

    public void addBid(int agentId, int amount){
        bidSet.put(agentId, amount);
    }

    public void setEndowment(int endowment) {
        this.endowment = endowment;
    }

    public int getWtp() {
        return wtp;
    }

    public void setWtp(int wtp) {
        this.wtp = wtp;
    }

    public LinkedList<Integer> getBidders() {
        return bidders;
    }

    public void setBidders(LinkedList<Integer> bidders) {
        this.bidders = bidders;
    }

    public double getAuctionId() {
        return auctionId;
    }

    public void setAuctionId(double auctionId) {
        this.auctionId = auctionId;
    }

    public int getAuctionIteration() {
        return auctionIteration;
    }

    public void setAuctionIteration(int auctionIteration) {
        this.auctionIteration = auctionIteration;
    }
}
