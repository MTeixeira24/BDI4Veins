package jasonveins.omnet.agent.voting;

import jasonveins.omnet.agent.CBargainAgent;
import jasonveins.omnet.agent.IVehicleAgent;
import jasonveins.omnet.agent.utilityFunctions.CGaussianUtility;
import jasonveins.omnet.agent.utilityFunctions.IUtilityFunction;
import jasonveins.omnet.constants.CVariableBuilder;
import jasonveins.omnet.decision.InstructionModel;
import jasonveins.omnet.environment.dijkstra.Graph;
import jasonveins.omnet.managers.AgentManager;
import jasonveins.omnet.managers.CAgentCreationQueue;
import jasonveins.omnet.managers.CVoterAgentManager;
import jasonveins.omnet.managers.constants.Constants;
import jasonveins.omnet.managers.constants.VoteConstants;
import jasonveins.omnet.voting.CContext;
import org.lightjason.agentspeak.action.binding.IAgentAction;
import org.lightjason.agentspeak.action.binding.IAgentActionFilter;
import org.lightjason.agentspeak.action.binding.IAgentActionName;
import org.lightjason.agentspeak.common.CCommon;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;
import org.lightjason.agentspeak.generator.IBaseAgentGenerator;
import org.lightjason.agentspeak.language.instantiable.plan.IPlan;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;
import java.util.stream.Stream;

@IAgentAction
public class CVoterAgent extends IVehicleAgent<CVoterAgent> {

    private CContext m_context;
    protected IUtilityFunction utility;
    private CVoteParameters votingState;
    private CommitteeRule committeeRule;
    private SingleRule singleRule;

    /**
     * ctor
     *  @param p_configuration agent configuration
     * @param m_am The agent manager of this agent
     * @param m_id The id of this agent
     * @param m_vType The vehicle type
     * @param voteRule
     * @param utility
     * @param committee_vote_rule
     */
    public CVoterAgent(@Nonnull IAgentConfiguration<CVoterAgent> p_configuration,
                       @Nonnull AgentManager m_am, int m_id, @Nonnull String m_vType, String voteRule,
                       String utility, String committee_vote_rule) {
        super(p_configuration, m_am, m_id, m_vType);
        singleRule = new SingleRule(voteRule);
        committeeRule = new CommitteeRule(committee_vote_rule);
        if(utility.equals("gaussian"))
            this.utility = new CGaussianUtility();
        else
            throw new RuntimeException("Unknown utility function type");

        agentManager.initRow(id);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "initialize/agent" )
    private void initializeAgent(Number p_isLeader, Number p_platoonSpeed,Number p_preferredSpeed,
                                 List<Integer> p_preferredRoute,List<Integer> p_members){
        votingState = new CVoteParameters(
                p_preferredSpeed.intValue(), ((CVoterAgentManager)agentManager).getRoute(),
                p_platoonSpeed.intValue(), p_isLeader.intValue(), null, p_members,
                p_preferredRoute
        );
        utility.setPreferredSpeed(p_preferredSpeed.intValue());
        utility.setPreferredRoute(p_preferredRoute);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "vote/open" )
    private void openVote(Number p_voteType, List<Number> contextArgs){
        System.out.println("Starting vote with id: " + p_voteType.shortValue());
        InstructionModel iOb = new InstructionModel(this.id, VoteConstants.NOTIFY_START_VOTE);
        ArrayList<Integer> l_candidates = new ArrayList<>();
        ArrayList<Integer> l_context = new ArrayList<>();
        ArrayList<Integer> l_context_chair = new ArrayList<>();


        switch (p_voteType.shortValue()){
            case VoteConstants.CONTEXT_JOIN:
                /*Add the necessary information for the vote*/
                iOb.pushInt(VoteConstants.CONTEXT_JOIN);
                /*Send speed (0) and preference(1) of joiner*/
                l_context.add(contextArgs.get(0).intValue());
                l_context.add(contextArgs.get(1).intValue());
                iOb.pushIntArray(l_context);
                /*Vote is a simple yes, no. Push those values*/
                l_candidates.add(0);
                l_candidates.add(1);
                /*Define the context for future reference*/
                m_context = new CContext(l_candidates, VoteConstants.CONTEXT_JOIN,
                        votingState.getMembers().size(), false);
                m_context.addContextArgument("joinerSpeed", contextArgs.get(0));
                m_context.addContextArgument("joinerPreference", contextArgs.get(1));
                m_context.addContextArgument("joinerId", contextArgs.get(2));
                /*Set up a data structure for the chair to cast its vote*/
                l_context_chair.add((int)VoteConstants.CONTEXT_JOIN);
                l_context_chair.addAll(l_context);
                break;
            case VoteConstants.CONTEXT_SPEED:{
                iOb.pushInt(VoteConstants.CONTEXT_SPEED);
                /*No context is needed*/
                iOb.pushShort(Constants.VALUE_NULL);
                /*Prepare a simple list of possible candidates*/
                for(int i = 80; i <= 120; i += 5){
                    l_candidates.add(i);
                }//80 85 90 95 100 105 110 115 120
                double cArg = contextArgs.get(0).intValue();
                if( cArg == 1){
                    //Alternate speed voting chosen. Remove the previously chosen speed
                    //and the other alternatives around it
                    l_candidates.removeIf(
                            s -> s >= votingState.getPlatoonSpeed() - 5 && s < votingState.getPlatoonSpeed() + 5
                    );
                }
                m_context = new CContext(l_candidates, VoteConstants.CONTEXT_SPEED,
                        votingState.getMembers().size(), true);
                l_context_chair.add((int)VoteConstants.CONTEXT_SPEED);
                break;
            }//platoonSpeed
            case VoteConstants.CONTEXT_PATH:{
                iOb.pushInt(VoteConstants.CONTEXT_PATH);
                //Get the environment data
                Graph route = ((CVoterAgentManager)agentManager).getRoute();
                if(contextArgs.get(0).intValue() == 1){
                    //Alternate route voting chosen. Remove the previously chosen route
                    //from the possible candidates
                    int[] toRemove = {5,6,7,8,9};
                    route.alterRoute(votingState.getCurrentPath(),toRemove);
                }
                //No special context needed
                iOb.pushShort(Constants.VALUE_NULL);
                //Gather all existing nodes to send to agents
                l_candidates.addAll(route.getVertexesMap().keySet());
                //Set up the context
                m_context = new CContext(l_candidates, VoteConstants.CONTEXT_PATH,
                        votingState.getMembers().size(), false);
                l_context_chair.add((int)VoteConstants.CONTEXT_PATH);
                break;
            }
            case VoteConstants.CONTEXT_REGROUP:{
                int contextStart = (contextArgs.size() / 2);
                //First half contains the candidates
                contextArgs.stream().limit(contextStart).forEach(
                        dvalue -> l_candidates.add(dvalue.intValue())
                );
                //Second half their speeds
                contextArgs.stream().skip(contextStart).forEach(
                        dvalue -> l_candidates.add(dvalue.intValue())
                );
                //First parameter, what are the agents voting for?
                iOb.pushInt(VoteConstants.CONTEXT_REGROUP);
                //What information do the agents need to know about the candidates? Their preferred speed
                iOb.pushIntArray(l_context);
                //Save the context for later use
                m_context = new CContext(l_candidates, VoteConstants.CONTEXT_REGROUP,
                        votingState.getMembers().size(), false);
                for(int i = 0 ; i < l_context.size(); i++){
                    m_context.addContextArgument(l_candidates.get(i).toString(), l_context.get(i));
                }
                //Create a data structure for the leader. The leader can cast its vote
                l_context_chair.add((int)VoteConstants.CONTEXT_REGROUP);
                l_context_chair.addAll(l_context);
                break;
            }
        }
        iOb.pushIntArray(l_candidates);
        if( (p_voteType.shortValue() == VoteConstants.CONTEXT_JOIN)  ||
                (p_voteType.shortValue() == VoteConstants.CONTEXT_SPEED ) )
            iOb.pushInt(singleRule.expectedVoteSize(l_candidates.size()));
        else
            iOb.pushInt(committeeRule.expectedVoteSize(l_candidates.size()));
        sendVoteSelf(l_candidates, l_context_chair);
        agentManager.addInstruction(iOb);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "vote/send" )
    private void sendVote(List<Integer> p_candidates, List<Integer> p_context){
        List<Integer> votes;
        short l_context = p_context.get(0).shortValue();
        if (l_context == VoteConstants.CONTEXT_SPEED || l_context == VoteConstants.CONTEXT_JOIN){
            votes = singleRule.generateVoteVector(p_candidates, utility);
        }else{
            votes = committeeRule.generateVoteVector(p_candidates, p_context, votingState.getPreferredPath());
        }
        InstructionModel iOb = new InstructionModel(this.id, VoteConstants.SUBMIT_VOTE);
        iOb.pushIntArray(new ArrayList<>(votes));
        agentManager.addInstruction(iOb);

    }

    @IAgentActionFilter
    @IAgentActionName(  name = "vote/store" )
    private void saveVote(List<Integer> vote){
        if(m_context.pushVotes(vote)){
            if(m_context.isIterative()  && !m_context.iterationOver()){
                System.out.println("PREPARING NEXT ITERATION");
                InstructionModel iOb = new InstructionModel(this.id, VoteConstants.NOTIFY_START_VOTE);
                List<Integer> newCandidates = singleRule.reduceCandidates(m_context);
                LinkedList<Integer> l_context_chair = new LinkedList<>();
                iOb.pushInt(m_context.getVoteType());
                if(m_context.getVoteType() == VoteConstants.CONTEXT_SPEED){
                    /*No context is needed*/
                    iOb.pushShort(Constants.VALUE_NULL);
                    l_context_chair.add((int)VoteConstants.CONTEXT_SPEED);
                }
                m_context = new CContext(newCandidates, VoteConstants.CONTEXT_SPEED,
                        votingState.getMembers().size(), true);
                iOb.pushIntArray(newCandidates);
                iOb.pushInt(singleRule.expectedVoteSize(newCandidates.size()));
                sendVoteSelf(newCandidates, l_context_chair);
                agentManager.addInstruction(iOb);
            }else{
                InstructionModel iOb = new InstructionModel(this.id, VoteConstants.SEND_VOTE_RESULTS);
                iOb.pushInt(-1);
                List<Integer> result;
                if(m_context.getVoteType() == VoteConstants.CONTEXT_PATH) {
                    result = committeeRule.determineWinner(m_context, votingState);
                    iOb.pushIntArray(result);
                    collectData(result,m_context.getVoteType());
                }else if( m_context.getVoteType() == VoteConstants.CONTEXT_REGROUP){
                    //Scenario 3 requires all votes to be sent over
                    iOb.pushInt(-1);
                    iOb.pushIntArray(new ArrayList<>(committeeRule.getApprovalSum(m_context)));
                }
                else{
                    int winner = singleRule.determineWinner(m_context);
                    if(winner < 0){
                        m_context.increaseVoterCount();
                        m_context.pushVotes(singleRule.tieBrakeVote(m_context, utility));
                        winner = singleRule.determineWinner(m_context);
                    }
                    iOb.pushInt(winner);
                    collectData(winner,m_context.getVoteType());
                }
                agentManager.addInstruction(iOb);
            }
        }else{
            agentManager.notifyOfReturn(0);
        }
    }


    @IAgentActionFilter
    @IAgentActionName(  name = "collect" )
    private void collectData(Object result, Number contextId){
        if(contextId.intValue() == VoteConstants.CONTEXT_SPEED){
            Integer speed = ((Integer) result);
            agentManager.updateRow(id, "SpeedUtility", String.valueOf(utility.computeUtilitySpeed(speed)));
        }
        if(contextId.intValue() == VoteConstants.CONTEXT_PATH){
            @SuppressWarnings("unchecked")
            List<Integer> route = ((List<Integer>) result);
            double util = utility.computeUtilityRouteBitVector(route,votingState.getRoute());
            agentManager.updateRow(id, "RouteUtility", String.valueOf(util));
        }
    }

    private void sendVoteSelf(List<Integer> p_candidates, List<Integer> p_context){
        //TODO: Add the vote from the chair straight to the ballot here
        List<Integer> votes;
        short l_context = p_context.get(0).shortValue();
        if (l_context == VoteConstants.CONTEXT_SPEED || l_context == VoteConstants.CONTEXT_JOIN){
            votes = singleRule.generateVoteVector(p_candidates, utility);
        }else{
            votes = committeeRule.generateVoteVector(p_candidates, p_context, votingState.getPreferredPath());
        }
        m_context.pushVotes(votes);

    }

    public static class CVoterAgentGenerator extends IBaseAgentGenerator<CVoterAgent> {
        private AgentManager agentManager;
        private CAgentCreationQueue insertionQueue;
        private String voteRule;
        private String utility;
        private String committee_vote_rule;
        /**
         * @param p_stream ASL code as any stream e.g. FileInputStream
         * @param p_am Reference to agent manager
         */
        public CVoterAgentGenerator(@Nonnull final InputStream p_stream, AgentManager p_am) throws Exception
        {
            this(p_stream, p_am, null, null, null, null);
        }

        public CVoterAgentGenerator(@Nonnull final InputStream p_stream, AgentManager p_am,
                                    CAgentCreationQueue p_insertionQueue, String voteRule, String utility,
                                    String committee_vote_rule) throws Exception
        {
            super(
                    // input ASL stream
                    p_stream,
                    Stream.concat(
                            CCommon.actionsFromPackage(),
                            CCommon.actionsFromAgentClass( CVoterAgent.class )
                    ).collect(Collectors.toSet()),
                    new CVariableBuilder()
            );
            agentManager = p_am;
            insertionQueue = p_insertionQueue;
            this.voteRule = voteRule;
            this.utility = utility;
            this.committee_vote_rule = committee_vote_rule;
        }

        @Nullable
        @Override
        public CVoterAgent generatesingle(@Nullable Object... p_data) {
            assert p_data != null;
            if(insertionQueue != null){
                CAgentCreationQueue.CAgentData agentData = insertionQueue.popAgent();
                final CVoterAgent agent = new CVoterAgent(m_configuration, agentManager, agentData.getId(),
                        agentData.getvType(), voteRule, utility, committee_vote_rule);
                agentManager.addAgentToMap(agent);
                return agent;
            }else
                return new CVoterAgent(m_configuration, agentManager,(int)p_data[0],(String)p_data[1], voteRule, utility, committee_vote_rule);
        }

        public Set<IPlan> getPlans(){
            return m_configuration.plans();
        }
    }
}
