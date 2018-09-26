package jasonveins.omnet.agent;

import cern.colt.bitvector.BitVector;
import jasonveins.omnet.decision.InstructionModel;
import jasonveins.omnet.managers.AgentManager;
import jasonveins.omnet.managers.CVoterAgentManager;
import jasonveins.omnet.managers.Constants;
import jasonveins.omnet.voting.CContext;
import jasonveins.omnet.voting.CStatistics;
import jasonveins.omnet.voting.CUtilityPair;
import jasonveins.omnet.voting.rule.*;
import org.lightjason.agentspeak.action.binding.IAgentAction;
import org.lightjason.agentspeak.action.binding.IAgentActionFilter;
import org.lightjason.agentspeak.action.binding.IAgentActionName;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;
import org.lightjason.agentspeak.language.CLiteral;
import org.lightjason.agentspeak.language.CRawTerm;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.CTrigger;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.ITrigger;

import javax.annotation.Nonnull;
import java.util.*;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.ThreadLocalRandom;

import jasonveins.omnet.managers.VoteConstants;

@IAgentAction
public final class CVoterAgent extends IVehicleAgent<CVoterAgent> {

    private static final long serialVersionUID = 3455114282889790324L;

    private CopyOnWriteArrayList<Integer> members;
    private List<BitVector> m_bitVotes;
    private List<Integer> m_candidates;
    private int m_committeeSize;
    /**
     * Stores an ordered linked list containing platoon Ids and their utilities
     */
    private final List<PlatoonUtilities> targetPlatoons;
    private int targetPlatoonIndex = 0;
    private final CopyOnWriteArrayList<Integer> targetPlatoonIds;
    private double platoonSpeed = 0;

    //Object voting group
    CContext m_context;
    //Object myvote
    IRule votingRule;

    private int m_minSpeedDeviation;
    private int m_maxSpeedDeviation;

    private ThreadLocalRandom numberGenerator;

    /**
     * ctor
     *
     * @param p_configuration agent configuration
     * @param m_am Reference to the agent manager
     * @param m_id Id to be assigned to the new agent
     * @param m_vType Vehicle type Id
     */
    public CVoterAgent(@Nonnull IAgentConfiguration<CVoterAgent> p_configuration, @Nonnull AgentManager m_am, int m_id,
                       @Nonnull String m_vType, int p_minSpeedDeviation, int p_maxSpeedDeviation, String voteRule) {
        super(p_configuration, m_am, m_id, m_vType);
        members = new CopyOnWriteArrayList<>();
        targetPlatoons = Collections.synchronizedList(new LinkedList<>());
        targetPlatoonIds = new CopyOnWriteArrayList<>();
        m_minSpeedDeviation = p_minSpeedDeviation;
        m_maxSpeedDeviation = p_maxSpeedDeviation;
        numberGenerator = ThreadLocalRandom.current();
        switch (voteRule){
            case "Borda":{
                votingRule = new CBorda();
                break;
            }
            case "Approval":{
                votingRule = new CApproval();
                break;
            }
            case "Plurality":{
                votingRule = new CPlurality();
                break;
            }
            case "Copeland":{
                votingRule = new CCopeland();
                break;
            }
            default:{
                throw new RuntimeException("CVoterAgent: Unknown voteRule!");
            }
        }
    }

    @IAgentActionFilter
    @IAgentActionName( name = "utility/save")
    private void saveUtils(@Nonnull Number w, @Nonnull Number t, @Nonnull Number s, @Nonnull Number p_newUtil){
        double oldUtil = calculateUtility(w.doubleValue(), platoonSpeed, t.doubleValue(), s.doubleValue());
        agentManager.getStats().setInitAndFinalUtil(this.id, oldUtil, p_newUtil.doubleValue());
    }

    @IAgentActionFilter
    @IAgentActionName( name = "utility/generate/utility" )
    private double calculateUtility(double welfareBonus, double speed, double tolerance, double preferedSpeed){
        double utility = welfareBonus / ( 1 + Math.pow(Math.abs( ( (speed - preferedSpeed)/(30*tolerance) ) ), 4) );
        /*Make the distinction later on data processing. Returning one will cause abnormal voting:
        i.e. 110 -> 1.1 120 -> 1.05 leads to voting as [4, 5] due to how sort() and the getVote() method works in Borda*/
        return utility ;//> 1 ? 1 : utility;
    }



    @IAgentActionFilter
    @IAgentActionName( name = "utility/generatetolerance" )
    private double generateTolerance()
    {
        return 0.7;//0.4 + (numberGenerator.nextInt(4) * 0.1);
    }

    @Nonnull
    @IAgentActionFilter
    @IAgentActionName( name = "utility/generatespeedpreference" )
    private double generatespeedpreference()
    {
        int steps = (m_maxSpeedDeviation - m_minSpeedDeviation)/5;
        return m_minSpeedDeviation + numberGenerator.nextInt(steps + 1)*5;
    }

    @IAgentActionFilter
    @IAgentActionName( name = "transmit/self/searchforplatoon" )
    private void searchForPlatoon()
    {
            /*InstructionModel iOb = new InstructionModel(this.id, Constants.REQUEST_SPEED_DOWN);
            iOb.pushInt(targetId.intValue());
            agentManager.addInstruction(iOb);*/
    }

    @IAgentActionFilter
    @IAgentActionName( name = "transmit/other/sendjoinplatoonrequest" )
    private void sendJoinPlatoonRequest(@Nonnull final Number platoonId, @Nonnull final Number leaderId, @Nonnull final Number speed,  @Nonnull final Number tolerance)
    {
            InstructionModel iOb = new InstructionModel(this.id, Constants.SEND_JOIN_REQUEST);
            iOb.pushInt(platoonId.intValue());
            iOb.pushInt(leaderId.intValue());
            iOb.pushDouble(speed.doubleValue());
            iOb.pushDouble(tolerance.doubleValue());
            agentManager.addInstruction(iOb);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "open/vote" )
    private void openNewVote(@Nonnull final String context, @Nonnull final List<Double> contextArgs){

        InstructionModel iOb = new InstructionModel(this.id, VoteConstants.NOTIFY_START_VOTE);
        ArrayList<Integer> l_candidates = new ArrayList<>();
        ArrayList<Double> l_context = new ArrayList<>();
        ArrayList<Double> l_context_chair = new ArrayList<>();
        switch (context){
            case "join":
                /*Add the necessary information for the vote*/
                iOb.pushInt(VoteConstants.CONTEXT_JOIN);
                l_context.add(contextArgs.get(0));
                l_context.add(contextArgs.get(1));
                iOb.pushDoubleArray(l_context);
                /*Vote is a simple yes, no. Push those values*/
                l_candidates.add(0);
                l_candidates.add(1);
                /*Define the context for future reference*/
                m_context = new CContext(l_candidates, VoteConstants.CONTEXT_JOIN, members.size());
                m_context.addContextArgument("joinerSpeed", contextArgs.get(0));
                m_context.addContextArgument("joinerPreference", contextArgs.get(1));
                m_context.addContextArgument("joinerId", contextArgs.get(2));
                /*Set up a data structure for the chair to cast its vote*/
                l_context_chair.add((double)VoteConstants.CONTEXT_JOIN);
                l_context_chair.addAll(l_context);
                break;
            case "speed":
                agentManager.getStats().setInitPlatoonSpeed((int)platoonSpeed);
                iOb.pushInt(VoteConstants.CONTEXT_SPEED);
                /*No context is needed*/
                iOb.pushShort(Constants.VALUE_NULL);
                /*Prepare a simple list of possible candidates*/
                for(int i = 80; i <= 120; i += 10){
                    l_candidates.add(i);
                }
                m_context = new CContext(l_candidates, VoteConstants.CONTEXT_SPEED, members.size());
                l_context_chair.add((double)VoteConstants.CONTEXT_SPEED);
                break;
        }
        iOb.pushIntArray(l_candidates);
        agentManager.addInstruction(iOb);
        final ITrigger l_trigger = CTrigger.from(
                ITrigger.EType.ADDGOAL,
                CLiteral.from(
                        "handle/vote/notification",
                        CRawTerm.from(l_candidates),
                        CRawTerm.from(l_context_chair)
                )

        );
        this.trigger( l_trigger );
    }

    /**
     * Generates a vote vector according to the selected voting rule
     * @param p_candidates A vector containing the possible candidates to vote on
     * @param p_tolerance This agents tolerance to preference deviations
     * @param p_speed This agents preferred speed
     * @param p_welfare Utility bonus for performing eco-driving
     * @return A vote vector, where vote casted occupies the same index as the candidate in the candidates vector
     */
    @IAgentActionFilter
    @IAgentActionName( name = "utility/generate/vote/vector" )
    private List<Integer> generateVoteVector(List<Integer> p_candidates, Number p_tolerance, Number p_speed, Number p_welfare, List<Double> p_context){
        ArrayList<Integer> l_candidates = new ArrayList<>(p_candidates.size());
        short l_context = (short)p_context.get(0).doubleValue();
        switch (l_context ){
            case VoteConstants.CONTEXT_JOIN: {
                l_candidates.add((int)platoonSpeed);
                l_candidates.add((int)predictPlatoonSpeed(p_context.get(1), p_context.get(2), platoonSpeed));
                break;
            }
            default:{
                l_candidates.addAll(p_candidates);
                break;
            }
        }
        List<CUtilityPair> utils = new ArrayList<>(l_candidates.size());
        for(int i = 0; i < l_candidates.size(); i++) {
            double util = calculateUtility(p_welfare.doubleValue(), l_candidates.get(i), p_tolerance.doubleValue(), p_speed.doubleValue());
            utils.add(new CUtilityPair(i, util));
        }
        List<Integer> votes = votingRule.getVote(utils);
        return votes;
    }

    @IAgentActionFilter
    @IAgentActionName( name = "utility/break/tie/vote" )
    private List<Integer> generateTieBreakerVote(List<Integer> p_candidates, Number p_tolerance, Number p_speed, Number p_welfare, List<Double> p_context, List<Integer> p_ties){
        ArrayList<Integer> l_candidates = new ArrayList<>(p_candidates.size());
        short l_context = (short)p_context.get(0).doubleValue();
        switch (l_context ){
            case VoteConstants.CONTEXT_JOIN: {
                l_candidates.add((int)platoonSpeed);
                l_candidates.add((int)predictPlatoonSpeed(p_context.get(1), p_context.get(2), platoonSpeed));
                break;
            }
            default:{
                l_candidates.addAll(p_candidates);
                break;
            }
        }
        List<CUtilityPair> utils = new ArrayList<>(l_candidates.size());
        for(int i = 0; i < l_candidates.size(); i++) {
            double util = calculateUtility(p_welfare.doubleValue(), l_candidates.get(i), p_tolerance.doubleValue(), p_speed.doubleValue());
            utils.add(new CUtilityPair(i, util));
        }
        List<Integer> votes = votingRule.getTieBreakerVote(utils, p_ties);
        return votes;
    }




    @IAgentActionFilter
    @IAgentActionName( name = "vote/store" )
    private void storeVote(@Nonnull final List<Integer> votes)
    {
        m_context.pushVotes(votes);
        if(m_context.allVotesSubmitted()){
            int winnerIndex = votingRule.getResult(m_context.getVotes(), m_context.getCandidates());
            if(winnerIndex == -1){
                m_context.increaseVoterCount();
                final ITrigger l_trigger = CTrigger.from(
                        ITrigger.EType.ADDGOAL,
                        CLiteral.from(
                                "handle/tie",
                                CRawTerm.from(m_context.getCandidates()),
                                CRawTerm.from(m_context.getContextSequence()),
                                CRawTerm.from(votingRule.getTiedIndexes())
                        )

                );
                this.trigger( l_trigger );
                return;
            }
            int winner = m_context.getCandidates().get(winnerIndex);
            System.out.println("Index that won is: " + winner);
            InstructionModel iOb = new InstructionModel(this.id, VoteConstants.SEND_VOTE_RESULTS);
            switch (m_context.getVoteType()){
                case VoteConstants.CONTEXT_JOIN:{
                    iOb.pushInt(m_context.getContextArgument("joinerId").intValue());
                    break;
                }
                case VoteConstants.CONTEXT_SPEED:{
                    agentManager.getStats().setFinalPlatoonSpeed(winner);
                    iOb.pushInt(-1);
                    final ITrigger l_trigger = CTrigger.from(
                            ITrigger.EType.ADDGOAL,
                            CLiteral.from(
                                    "handle/results",
                                    CRawTerm.from(winner)
                            )

                    );
                    this.trigger( l_trigger );
                    break;
                }
                default:
                    break;
            }
            iOb.pushInt(winner);
            agentManager.addInstruction(iOb);
        }
    }

    /**
     * Used by joiner agents to get the platoonId of the next viable platoon to join
     * @return Id of the next platoon to attempt negotiations. -1 if end of list has been reached
     */
    @IAgentActionFilter
    @IAgentActionName( name = "utility/next/platoon" )
    private int getNextPlatoon(){
        if(targetPlatoonIndex < targetPlatoons.size())
            return targetPlatoons.get(targetPlatoonIndex++).platoonId();
        agentManager.getStats().setRejected(true);
        return -1;
    }

    /**
     * Used by joiner agents to store information about potential platoons to join
     * insertion is ordered for greatest to utility to lowest
     * @param p_platoonId The id of the platoon
     * @param p_platoonSpeed The broadcasted platoon speed to use in utility calculation
     * @param tolerance The tolerance this agent has for preference deviations
     * @param p_preferredSpeed The preferred speed
     */
    @IAgentActionFilter
    @IAgentActionName( name = "utility/store/platoon")
    private void storeTargetPlatoon(@Nonnull Number p_platoonId, @Nonnull Number p_platoonSpeed, @Nonnull Number p_leaderId,@Nonnull Number tolerance, @Nonnull Number p_preferredSpeed){
        double util = calculateUtility(1, p_platoonSpeed.doubleValue() * 3.2, tolerance.doubleValue(), p_preferredSpeed.doubleValue());
        PlatoonUtilities pu = new PlatoonUtilities(p_platoonId.intValue(), util, p_leaderId.intValue());
        synchronized (targetPlatoons){
            if(!targetPlatoonIds.contains(p_platoonId.intValue())){
                Iterator<PlatoonUtilities> I = targetPlatoons.iterator();
                int counter = 0;
                while(I.hasNext()){
                    if( I.next().utility() < util ){
                        targetPlatoons.add(counter, pu);
                        targetPlatoonIds.add(p_platoonId.intValue());
                        return;
                    }
                    counter++;
                }
                targetPlatoons.add(pu);
                targetPlatoonIds.add(p_platoonId.intValue());
            }
        }
    }

    @IAgentActionFilter
    @IAgentActionName( name = "utility/get/leader")
    private int getLeaderOfPlatoon(@Nonnull Number p_platoonId){
        for (PlatoonUtilities targetPlatoon : targetPlatoons) {
            if (targetPlatoon.platoonId() == p_platoonId.intValue()) {
                return targetPlatoon.leaderId();
            }
        }
        return -1;
    }

    @IAgentActionFilter
    @IAgentActionName( name = "utility/predictplatoonspeed" )
    private double predictPlatoonSpeed(@Nonnull final Number jspeed,  @Nonnull final Number jtolerance, @Nonnull final Number pspeed)
    {
        double expectedSpeedVariance = pspeed.doubleValue() + ((jspeed.doubleValue() - pspeed.doubleValue())*(1-jtolerance.doubleValue()));

        return expectedSpeedVariance;
    }

    @IAgentActionFilter
    @IAgentActionName( name = "utility/set/platoon/speed" )
    private void setPlatoonSpeed(@Nonnull final Number p_platoonSpeed)
    {
        platoonSpeed = p_platoonSpeed.doubleValue();
    }

    @IAgentActionFilter
    @IAgentActionName( name = "utility/storemember" )
    private void storeMember(@Nonnull final Number memberId)
    {
        members.add(memberId.intValue());
    }

    @IAgentActionFilter
    @IAgentActionName( name = "transmit/other/vote/list" )
    private void sendVote(@Nonnull final List<Integer> vote)
    {
        InstructionModel iOb = new InstructionModel(this.id, VoteConstants.SUBMIT_VOTE);
        iOb.pushIntArray(new ArrayList<>(vote));
        agentManager.addInstruction(iOb);
    }

    private class PlatoonUtilities{
        private int m_platoonId;
        private double m_utility;
        private int m_leaderId;

        PlatoonUtilities(int p_platoonid, double p_utility, int p_leaderId){
            m_platoonId = p_platoonid;
            m_utility = p_utility;
            m_leaderId = p_leaderId;
        }

        int platoonId(){return m_platoonId;}

        double utility(){return m_utility;}

        int leaderId(){return m_leaderId;}
    }
}
