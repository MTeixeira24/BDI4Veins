package jasonveins.omnet.agent;

import cern.colt.bitvector.BitVector;
import jasonveins.omnet.decision.InstructionModel;
import jasonveins.omnet.environment.dijkstra.Dijkstra;
import jasonveins.omnet.environment.dijkstra.Graph;
import jasonveins.omnet.environment.dijkstra.Vertex;
import jasonveins.omnet.managers.AgentManager;
import jasonveins.omnet.managers.CVoterAgentManager;
import jasonveins.omnet.managers.Constants;
import jasonveins.omnet.voting.CContext;
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
import java.util.concurrent.atomic.AtomicInteger;

import jasonveins.omnet.managers.VoteConstants;

@IAgentAction
public final class CVoterAgent extends IVehicleAgent<CVoterAgent> {

    private static final long serialVersionUID = 3455114282889790324L;

    private CopyOnWriteArrayList<Integer> members;
    /**
     * Stores an ordered linked list containing platoon Ids and their utilities
     */
    private final List<PlatoonUtilities> targetPlatoons;
    private int targetPlatoonIndex = 0;
    private final CopyOnWriteArrayList<Integer> targetPlatoonIds;
    private double platoonSpeed = 0;
    private int hammingDistance = 0;

    private CContext m_context;
    private IRule votingRule;
    private IRule committeeRule;
    private double factor;
    private String utility;
    private LinkedList<Vertex> currentPath;

    /**
    * Used by the leader to compute optimal path
    */
    private Graph route;

    /**
     * ctor
     *
     * @param p_configuration agent configuration
     * @param m_am Reference to the agent manager
     * @param m_id Id to be assigned to the new agent
     * @param m_vType Vehicle type Id
     */
    public CVoterAgent(@Nonnull IAgentConfiguration<CVoterAgent> p_configuration, @Nonnull AgentManager m_am, int m_id,
                       @Nonnull String m_vType, String voteRule, double m_factor, String m_utility, String m_committee_vote_rule) {
        super(p_configuration, m_am, m_id, m_vType);
        members = new CopyOnWriteArrayList<>();
        targetPlatoons = Collections.synchronizedList(new LinkedList<>());
        targetPlatoonIds = new CopyOnWriteArrayList<>();
        factor = m_factor;
        utility = m_utility;
        currentPath = null;
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
        switch (m_committee_vote_rule){
            case "Minisum":{
                committeeRule = new CMinisum();
                break;
            }
            case "Minimax":{
                committeeRule = new CMinimax();
                break;
            }
            default:{
                throw new RuntimeException("CVoterAgent: Unknown committeeRule!");
            }
        }
    }

    @IAgentActionFilter
    @IAgentActionName( name = "utility/get/nodes")
    private void getNodes(){
        route = ((CVoterAgentManager)agentManager).getRoute();
    }

    @IAgentActionFilter
    @IAgentActionName( name = "utility/save")
    private void saveUtils(@Nonnull Number t, @Nonnull Number s, @Nonnull Number p_newUtil, @Nonnull Number p_currentSpeed){
        double oldUtil = calculateUtility(platoonSpeed, t.doubleValue(), s.doubleValue(), p_currentSpeed.intValue());
        ((CVoterAgentManager)agentManager).getStats().setInitAndFinalUtil(this.id, oldUtil, p_newUtil.doubleValue(), hammingDistance);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "utility/generate/utility" )
    private double calculateUtility(double offeredSpeed, double factor, double preferredSpeed, int currentSpeed){

        switch (utility){
            case "khan":{
                return khanUtilility(currentSpeed, (int)offeredSpeed, (int)preferredSpeed, factor);
            }
            case "distribution":{
                return normalUtility(1.1, offeredSpeed, factor, preferredSpeed);
            }
            default:
                return -1;
        }
    }

    /**
     * Get the hamming distance
     * @param results results of the election
     * @param nodes the preferred nodes of this agent
     * @return the hamming distance
     */
    @IAgentActionFilter
    @IAgentActionName( name = "utility/generate/utility/committee" )
    private int calculateCommitteeUtility(List<Integer> results, List<Integer> nodes){
        ArrayList<Integer> preferredVector = new ArrayList<>(Collections.nCopies(results.size(), 0));
        Set<Integer> vertexes = ((CVoterAgentManager)agentManager).getRoute().getVertexesMap().keySet();
        Iterator<Integer> it = vertexes.iterator();
        int position = 0;
        while(it.hasNext()){
            int node = it.next();
            if(nodes.contains(node))
                preferredVector.set(position, 1);
            position++;
        }
        /*
        [1,1,0,0,1,1,1]
        [2,4,7,8]
        [0,0,0,0,0,0,0]
         */
        //get hamming distance, inverted in order to work with preexisting plans
        int hammingDistance = results.size();
        for(int i = 0; i < results.size(); i++){
            if(!results.get(i).equals(preferredVector.get(i))) hammingDistance--;
        }
        this.hammingDistance = hammingDistance;
        return hammingDistance;
    }

    private double normalUtility(double welfareBonus, double speed, double tolerance, double preferedSpeed){
        return  welfareBonus / ( 1 + Math.pow(Math.abs( ( (speed - preferedSpeed)/(30*tolerance) ) ), 4) );
    }

    private double khanUtilility(double currentSpeed, double offeredSpeed, double desiredSpeed, double joinFactor){
        double d1 = Math.abs(desiredSpeed - offeredSpeed)/desiredSpeed;
        double d2 = Math.abs(currentSpeed/offeredSpeed)/currentSpeed;
        double result = 1 - d1 - factor*d2;
        return  result;
    }



    @IAgentActionFilter
    @IAgentActionName( name = "utility/generatetolerance" )
    private double generateTolerance()
    {
        return 0.7;//0.4 + (numberGenerator.nextInt(4) * 0.1);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "utility/get/factor" )
    private double getFactor()
    {
        return factor;
    }


    @IAgentActionFilter
    @IAgentActionName( name = "test/debug" )
    private void debug()
    {
            System.out.println("Test");
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
                /*Send speed (0) and preference(1) of joiner*/
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
                ((CVoterAgentManager)agentManager).getStats().setInitPlatoonSpeed((int)platoonSpeed);
                iOb.pushInt(VoteConstants.CONTEXT_SPEED);
                /*No context is needed*/
                iOb.pushShort(Constants.VALUE_NULL);
                /*Prepare a simple list of possible candidates*/
                for(int i = 80; i <= 120; i += 5){
                    l_candidates.add(i);
                }
                m_context = new CContext(l_candidates, VoteConstants.CONTEXT_SPEED, members.size());
                l_context_chair.add((double)VoteConstants.CONTEXT_SPEED);
                break;
            case "node":{
                iOb.pushInt(VoteConstants.CONTEXT_PATH);
                //Get the environment data
                route = ((CVoterAgentManager)agentManager).getRoute();
                if(contextArgs.get(0) == 1){
                    //Alternate route voting chosen. Remove the previously chosen route
                    //from the possible candidates
                    int[] toRemove = {5,6,7,8,9};
                    route.alterRoute(currentPath,toRemove);
                }
                //No special context needed
                iOb.pushShort(Constants.VALUE_NULL);
                //Gather all existing nodes to send to agents
                l_candidates.addAll(route.getVertexesMap().keySet());
                //Set up the context
                m_context = new CContext(l_candidates, VoteConstants.CONTEXT_PATH, members.size());
                l_context_chair.add((double)VoteConstants.CONTEXT_PATH);
                break;
            }
        }
        iOb.pushIntArray(l_candidates);
        //Add additional information
        switch (context){
            case "join":
            case "speed":
                iOb.pushInt(votingRule.getExpectedVoteSize(l_candidates.size()));
                break;
            case "node":
                iOb.pushInt(committeeRule.getExpectedVoteSize(l_candidates.size()));
                break;
            default:
                break;
        }
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
     * @param p_currentSpeed Current cruising speed
     * @return A vote vector, where vote casted occupies the same index as the candidate in the candidates vector
     */
    @IAgentActionFilter
    @IAgentActionName( name = "utility/generate/vote/vector" )
    private List<Integer> generateVoteVector(List<Integer> p_candidates, Number p_tolerance, Number p_speed, Number p_currentSpeed, List<Double> p_context, List<Integer> nodes){
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
        List<Integer> votes;
        switch (l_context){
            case VoteConstants.CONTEXT_PATH:{
                for(int i = 0; i < l_candidates.size(); i++){
                    if(nodes.contains(l_candidates.get(i)))
                        utils.add(new CUtilityPair(i, 1));
                    else
                        utils.add(new CUtilityPair(i, 0));
                }
                votes = committeeRule.getVote(utils);
                break;
            }
            default:{
                for(int i = 0; i < l_candidates.size(); i++) {
                    double util = calculateUtility(l_candidates.get(i), p_tolerance.doubleValue(), p_speed.doubleValue(), p_currentSpeed.intValue());
                    utils.add(new CUtilityPair(i, util));
                }
                votes = votingRule.getVote(utils);
                break;
            }
        }

        return votes;
    }

    @IAgentActionFilter
    @IAgentActionName( name = "utility/break/tie/vote" )
    private List<Integer> generateTieBreakerVote(List<Integer> p_candidates, Number p_tolerance, Number p_speed, Number p_currentSpeed, List<Double> p_context, List<Integer> p_ties){
        ArrayList<Integer> l_candidates = new ArrayList<>(p_candidates.size());
        short l_context = (short)p_context.get(0).doubleValue();
        switch (l_context ){
            case VoteConstants.CONTEXT_JOIN: {
                l_candidates.add((int)platoonSpeed);
                l_candidates.add((int)predictPlatoonSpeed(p_context.get(1), p_context.get(3), platoonSpeed));
                break;
            }
            default:{
                l_candidates.addAll(p_candidates);
                break;
            }
        }
        List<CUtilityPair> utils = new ArrayList<>(l_candidates.size());
        for(int i = 0; i < l_candidates.size(); i++) {
            double util = calculateUtility(l_candidates.get(i), p_tolerance.doubleValue(), p_speed.doubleValue(), p_currentSpeed.intValue());
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
        m_context.debug();
        if(m_context.allVotesSubmitted()){
            if(m_context.getVoteType() == VoteConstants.CONTEXT_PATH){
                committeeWinnerDetermination();
            }else{
                singleCandidateWinnerDetermination();
            }

        }
    }

    private void singleCandidateWinnerDetermination(){
        System.out.println("FINAL ITERATION");
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
                iOb.pushInt(winner);
                break;
            }
            case VoteConstants.CONTEXT_SPEED:{
                ((CVoterAgentManager)agentManager).getStats().setFinalPlatoonSpeed(winner);
                iOb.pushInt(-1);
                final ITrigger l_trigger = CTrigger.from(
                        ITrigger.EType.ADDGOAL,
                        CLiteral.from(
                                "handle/results",
                                CRawTerm.from(winner)
                        )

                );
                this.trigger( l_trigger );
                iOb.pushInt(winner);
                break;
            }
            default:
                break;
        }
        agentManager.addInstruction(iOb);
    }

    private void committeeWinnerDetermination(){
        System.out.println("COMMITTEE WINNER DETERMINATION");
        ArrayList<Integer> results = new ArrayList<>(committeeRule.getResultVector(m_context.getVotes(), m_context.getCandidates(), 5));
        InstructionModel iOb = new InstructionModel(this.id, VoteConstants.SEND_VOTE_RESULTS);
        iOb.pushInt(-1);
        if(m_context.getVoteType() == VoteConstants.CONTEXT_PATH){
            //Update node values
            for(int i = 0; i < route.getVertexes().size(); i++){
                route.getVertexes().get(i).setValue(results.get(i));
            }
            //Update the graphs weights
            route.updateWeights(Collections.max(results));
            //Get the route
            Dijkstra dijkstra = new Dijkstra(route);
            //Assume that the origin is at the beginning of the config file and the destination at the end of the config file
            dijkstra.execute(route.getVertexes().get(0));
            LinkedList<Vertex> path = dijkstra.getPath(route.getVertexes().get(route.getVertexes().size() - 1));
            //Store the path to take
            currentPath = path;
            //Convert to a score list
            ArrayList<Integer> pathIds = new ArrayList<>();
            for (Vertex aPath : path) {
                pathIds.add(Integer.parseInt(aPath.getId()));
            }
            ArrayList<Integer> scoreVector = new ArrayList<>();
            for(int i = 0; i < m_context.getCandidates().size(); i++){
                if(pathIds.contains(m_context.getCandidates().get(i)))
                    scoreVector.add(1);
                else
                    scoreVector.add(0);
            }
            iOb.pushIntArray(scoreVector);

            final ITrigger l_trigger = CTrigger.from(
                    ITrigger.EType.ADDGOAL,
                    CLiteral.from(
                            "handle/results/committee",
                            CRawTerm.from(scoreVector)
                    )

            );
            this.trigger( l_trigger );
        }else{
            iOb.pushIntArray(results);
        }
        agentManager.addInstruction(iOb);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "vote/store/iterative" )
    private void storeVoteIterative(@Nonnull final List<Integer> votes)
    {
        if(votes.size() > votingRule.getExpectedVoteSize(getCandidateListSize())) return;
        //TODO: Change to iterative
        if((getCandidateListSize() <= 2) || m_context.getVoteType() == VoteConstants.CONTEXT_PATH ){
            storeVote(votes);
            return;
        }
        m_context.pushVotes(votes);
        if(m_context.allVotesSubmitted()){
            System.out.println("PREPARING NEXT ITERATION");
            ArrayList<Integer> newCandidates = (ArrayList<Integer>)votingRule.reduceCandidates(m_context.getVotes(), m_context.getCandidates());
            InstructionModel iOb = new InstructionModel(this.id, VoteConstants.NOTIFY_START_VOTE);
            ArrayList<Double> l_context_chair = new ArrayList<>();
            iOb.pushInt(m_context.getVoteType());
            switch (m_context.getVoteType()){
                case VoteConstants.CONTEXT_SPEED:{
                    /*No context is needed*/
                    iOb.pushShort(Constants.VALUE_NULL);
                    l_context_chair.add((double)VoteConstants.CONTEXT_SPEED);
                }
            }
            m_context = new CContext(newCandidates, VoteConstants.CONTEXT_SPEED, members.size());
            iOb.pushIntArray(newCandidates);
            iOb.pushInt(votingRule.getExpectedVoteSize(newCandidates.size()));
            agentManager.addInstruction(iOb);
            final ITrigger l_trigger = CTrigger.from(
                    ITrigger.EType.ADDGOAL,
                    CLiteral.from(
                            "handle/vote/notification",
                            CRawTerm.from(newCandidates),
                            CRawTerm.from(l_context_chair)
                    )

            );
            this.trigger( l_trigger );
        }
    }

    @IAgentActionFilter
    @IAgentActionName(name = "utility/get/candidates/size")
    public int getCandidateListSize(){
        return m_context.getCandidates().size();
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
        ((CVoterAgentManager)agentManager).getStats().setRejected(true);
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
    private void storeTargetPlatoon(@Nonnull Number p_platoonId, @Nonnull Number p_platoonSpeed, @Nonnull Number p_leaderId,@Nonnull Number tolerance, @Nonnull Number p_preferredSpeed, @Nonnull Number p_currentSpeed){
        double util = calculateUtility(p_platoonSpeed.intValue(), tolerance.doubleValue(), p_preferredSpeed.doubleValue(), p_currentSpeed.intValue());
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
    @IAgentActionName( name = "utility/store/platoon/start")
    private void storeTargetPlatoonAndStart(@Nonnull Number p_platoonId, @Nonnull Number p_platoonSpeed, @Nonnull Number p_leaderId,@Nonnull Number tolerance, @Nonnull Number p_preferredSpeed, @Nonnull Number p_currentSpeed){
        storeTargetPlatoon(p_platoonId, p_platoonSpeed, p_leaderId, tolerance, p_preferredSpeed, p_currentSpeed);
        sendJoinPlatoonRequest(p_platoonId, p_leaderId, p_preferredSpeed,  tolerance);
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
        return pspeed.doubleValue() + ((jspeed.doubleValue() - pspeed.doubleValue())*(1-jtolerance.doubleValue()));
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
        if(memberId.intValue() > -1 ) members.add(memberId.intValue());
    }

    @IAgentActionFilter
    @IAgentActionName( name = "utility/storememberlist" )
    private void storeMemberList(@Nonnull final List<Integer> members/*, @Nonnull final List<Integer> speeds*/)
    {
        this.members.addAll(members);
        //this.membersSpeeds.addAll(speeds);
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
