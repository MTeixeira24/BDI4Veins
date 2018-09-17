package jasonveins.omnet.agent;

import cern.colt.bitvector.BitVector;
import jasonveins.omnet.decision.InstructionModel;
import jasonveins.omnet.managers.AgentManager;
import jasonveins.omnet.managers.Constants;
import jasonveins.omnet.voting.rule.CBorda;
import org.lightjason.agentspeak.action.binding.IAgentAction;
import org.lightjason.agentspeak.action.binding.IAgentActionFilter;
import org.lightjason.agentspeak.action.binding.IAgentActionName;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;
import org.lightjason.agentspeak.language.CLiteral;
import org.lightjason.agentspeak.language.CRawTerm;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.CTrigger;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.ITrigger;

import javax.annotation.Nonnull;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Random;
import java.util.concurrent.CopyOnWriteArrayList;

@IAgentAction
public final class CVoterAgent extends IVehicleAgent<CVoterAgent> {

    private static final long serialVersionUID = 3455114282889790324L;

    private CopyOnWriteArrayList<Integer> members;
    private List<BitVector> m_bitVotes;
    private List<Integer> m_candidates;
    private int m_committeeSize;

    //Object voting group
    //Object voting context
    //Object myvote
    //Object winner determination

    /**
     * ctor
     *
     * @param p_configuration agent configuration
     * @param m_am Reference to the agent manager
     * @param m_id Id to be assigned to the new agent
     * @param m_vType Vehicle type Id
     */
    public CVoterAgent(@Nonnull IAgentConfiguration<CVoterAgent> p_configuration, @Nonnull AgentManager m_am, int m_id, @Nonnull String m_vType) {
        super(p_configuration, m_am, m_id, m_vType);
        members = new CopyOnWriteArrayList<>();
    }

    private double calculateUtility(double welfareBonus, double speed, double tolerance, double preferedSpeed){
        double utility = welfareBonus / ( 1 + Math.pow(Math.abs( ( (speed - preferedSpeed)/(30*tolerance) ) ), 4) );
        return utility > 1 ? 1 : utility;
    }


    @IAgentActionFilter
    @IAgentActionName( name = "utility/generatetolerance" )
    private double generateTolerance()
    {
        Random r = new Random();
        return 0.4 + (r.nextInt(4) * 0.1);
    }

    @Nonnull
    @IAgentActionFilter
    @IAgentActionName( name = "utility/generatespeedpreference" )
    private double generatespeedpreference()
    {
        Random r = new Random();
        return 70 + r.nextInt(3)*10;
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
    @IAgentActionName( name = "transmit/other/vote/join" )
    private void notifyMembersOfJoinVote(@Nonnull final Number speed,  @Nonnull final Number tolerance)
    {
        InstructionModel iOb = new InstructionModel(this.id, Constants.NOTIFY_START_VOTE_JOIN);
        iOb.pushDouble(speed.doubleValue());
        iOb.pushDouble(tolerance.doubleValue());
        agentManager.addInstruction(iOb);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "utility/predictplatoonspeed" )
    private double predictPlatoonSpeed(@Nonnull final Number jspeed,  @Nonnull final Number jtolerance, @Nonnull final Number speed,  @Nonnull final Number tolerance, @Nonnull final Number pspeed)
    {
        double expectedSpeedVariance = pspeed.doubleValue() + ((jspeed.doubleValue() - pspeed.doubleValue())*(1-jtolerance.doubleValue()));

        return calculateUtility(1.1, expectedSpeedVariance, tolerance.doubleValue(), speed.doubleValue());
    }

    @IAgentActionFilter
    @IAgentActionName( name = "utility/storemember" )
    private void storeMember(@Nonnull final Number memberId)
    {
        members.add(memberId.intValue());
    }
    @IAgentActionFilter
    @IAgentActionName( name = "utility/platoonsize" )
    private int platoonSize()
    {
        return members.size();
    }

    @IAgentActionFilter
    @IAgentActionName( name = "transmit/other/vote/cast" )
    private void sendVote(@Nonnull final Number vote)
    {
        InstructionModel iOb = new InstructionModel(this.id, Constants.SUBMIT_VOTE);
        iOb.pushInt(vote.intValue());
        agentManager.addInstruction(iOb);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "vote/openballot" )
    private void openBallot(@Nonnull final String type, @Nonnull final Number candidate, @Nonnull final Number committeeSize)
    {
        switch (type){
            case "allowJoin":
                m_candidates = Collections.synchronizedList( new LinkedList<>() );
                m_candidates.add(-1);
                m_candidates.add(candidate.intValue());
                break;
        }
        m_bitVotes = Collections.synchronizedList( new LinkedList<>() );
        m_committeeSize = committeeSize.intValue();
    }

    @IAgentActionFilter
    @IAgentActionName( name = "vote/chairstore" )
    private void storeChairVote(@Nonnull final Number vote){
        storeVote(this.id, vote);
    }

    @IAgentActionFilter
    @IAgentActionName( name = "vote/store" )
    private void storeVote(@Nonnull final Number voter, @Nonnull final Number vote)
    {
        if(members.contains(voter.intValue())){
            BitVector bv = new BitVector(2);
            if(vote.intValue() == 1){
                bv.put(0, false);
                bv.put(1, true);
            }else{
                bv.put(0, true);
                bv.put(1, false);
            }
            m_bitVotes.add(bv);
        }
        if (m_bitVotes.size() == m_committeeSize){
            CBorda br = new CBorda(m_candidates, m_bitVotes);
            int winner = br.getResult();
            String functor;
            if(winner == 1){
                functor = "notify/joiner/win";
            }else{
                functor = "notify/joiner/lose";
            }
            final ITrigger l_trigger = CTrigger.from(
                ITrigger.EType.ADDGOAL,
                CLiteral.from(
                        functor,
                        CRawTerm.from( m_candidates.get(1) )
                )

            );
            this.trigger( l_trigger );
        }
    }
    @IAgentActionFilter
    @IAgentActionName( name = "vote/send/results" )
    private void sendVoteResults(@Nonnull final Number id, @Nonnull final Number result){
        InstructionModel iOb = new InstructionModel(this.id, Constants.SEND_VOTE_RESULTS);
        iOb.pushInt(id.intValue());
        iOb.pushInt(result.intValue());
        agentManager.addInstruction(iOb);
    }


    @IAgentActionFilter
    @IAgentActionName( name = "utility/next/platoon" )
    private int getNextPlatoon(){
        return -1;
    }
}
