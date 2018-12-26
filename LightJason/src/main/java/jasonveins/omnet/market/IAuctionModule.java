package jasonveins.omnet.market;

import jasonveins.omnet.decision.InstructionModel;
import jasonveins.omnet.managers.constants.MarketConstants;

import java.util.*;

public abstract class IAuctionModule {
    private int agentId;
    private int managerId;
    private int endowment;
    private int wtp;
    private LinkedList<Integer> bidders;
    private ArrayList<Integer> winnerSet;
    HashMap<Integer, Integer> bidSet;
    protected int winner;
    private int auctionId;
    private int auctionIteration;
    private Random rd;
    private int currentBid;

    public IAuctionModule(int agentId, int _endowment, int _wtp, int _auctionId){
        rd = new Random();
        endowment = _endowment;
        wtp = _wtp;
        auctionId  = _auctionId;
        auctionIteration = 0;
        this.agentId = agentId;
    }

    public IAuctionModule(int agentId){
        this(agentId, 100, 80, 0);
    }

    public InstructionModel createBidInstruction(int roundStatus){
        int value = castBid(roundStatus);
        System.out.println("Bidding with a price of " + value);
        InstructionModel iOb = new InstructionModel(agentId, MarketConstants.SUBMIT_BID);
        iOb.pushInt(auctionId);
        iOb.pushShort(MarketConstants.CONTEXT_SPEED);
        iOb.pushInt(value);
        iOb.pushInt(managerId);
        return iOb;
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

    public InstructionModel setupAuction(int context, List<Integer> participants){
        switch (context){
            case MarketConstants.CONTEXT_SPEED:{
                bidders = new LinkedList<>(participants);
                break;
            }
            default:{
                throw new RuntimeException("AuctionModule: Unknown context");
            }
        }
        auctionId = rd.nextInt();
        auctionIteration = 0;
        InstructionModel iOb = new InstructionModel(agentId, MarketConstants.NOTIFY_START_AUCTION);
        iOb.pushInt(auctionId);
        iOb.pushShort(MarketConstants.CONTEXT_SPEED);
        return iOb;
    }

    public InstructionModel createWinnerInstruction(){
        InstructionModel iOb = new InstructionModel(agentId, MarketConstants.SEND_AUCTION_RESULTS);
        iOb.pushInt(auctionId);
        auctionIteration++;
        iOb.pushInt(auctionIteration);
        iOb.pushInt(determineWinner());
        return iOb;
    }

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

    public void setAuctionId(int auctionId) {
        this.auctionId = auctionId;
    }

    public int getAuctionIteration() {
        return auctionIteration;
    }

    public void setAuctionIteration(int auctionIteration) {
        this.auctionIteration = auctionIteration;
    }

    public void setManagerId(int managerId) {
        this.managerId = managerId;
    }
}
