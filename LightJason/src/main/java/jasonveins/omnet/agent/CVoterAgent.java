package jasonveins.omnet.agent;

import jasonveins.omnet.constants.CVariableBuilder;
import jasonveins.omnet.decision.InstructionModel;
import jasonveins.omnet.managers.AgentManager;
import jasonveins.omnet.managers.Constants;
import org.lightjason.agentspeak.action.binding.IAgentAction;
import org.lightjason.agentspeak.action.binding.IAgentActionFilter;
import org.lightjason.agentspeak.action.binding.IAgentActionName;
import org.lightjason.agentspeak.common.CCommon;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;
import org.lightjason.agentspeak.generator.IBaseAgentGenerator;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.io.InputStream;
import java.util.Random;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicIntegerArray;
import java.util.stream.Collectors;
import java.util.stream.Stream;

@IAgentAction
public final class CVoterAgent extends IVehicleAgent<CVoterAgent> {

    private static final long serialVersionUID = 3455114282889790324L;

    private CopyOnWriteArrayList<Integer> members;

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
        return 0.3 + (r.nextInt(4) * 0.1);
    }

    @Nonnull
    @IAgentActionFilter
    @IAgentActionName( name = "utility/generatespeedpreference" )
    private double generatespeedpreference()
    {
        Random r = new Random();
        return 80 + r.nextInt(4)*10;
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
        /*InstructionModel iOb = new InstructionModel(this.id, Constants.SUBMIT_VOTE);
        iOb.pushInt(vote.intValue());
        agentManager.addInstruction(iOb);*/
    }
}
