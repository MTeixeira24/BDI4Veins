package jasonveins.omnet.agent;

import jasonveins.omnet.constants.CVariableBuilder;
import jasonveins.omnet.decision.InstructionModel;
import jasonveins.omnet.managers.AgentManager;
import jasonveins.omnet.managers.constants.BargainConstants;
import org.lightjason.agentspeak.action.binding.IAgentAction;
import org.lightjason.agentspeak.action.binding.IAgentActionFilter;
import org.lightjason.agentspeak.action.binding.IAgentActionName;
import org.lightjason.agentspeak.common.CCommon;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;
import org.lightjason.agentspeak.generator.IBaseAgentGenerator;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.io.InputStream;
import java.util.stream.Collectors;
import java.util.stream.Stream;

@IAgentAction
public class CBargainAgent extends IVehicleAgent<CBargainAgent> {


    private static final long serialVersionUID = -6952902911423402449L;
    private static final short NONE = -1;
    private static final short BARGAIN_START = 0;
    private static final short OFFER_REJECTED = 1;
    private static final short OFFER_ACCEPTED = 2;
    private static final short OFFER_SENT = 3;
    private static final short BARGAIN_END = 4;

    private int acceptThreshold;
    private int startingOffer;
    private int currentOffer;
    private int amountReceived;
    private short status;

    /**
     * ctor
     *
     * @param p_configuration agent configuration
     * @param m_am
     * @param m_id
     * @param m_vType
     */
    public CBargainAgent(@Nonnull IAgentConfiguration<CBargainAgent> p_configuration, @Nonnull AgentManager m_am, @Nonnull int m_id, @Nonnull String m_vType) {
        super(p_configuration, m_am, m_id, m_vType);
        acceptThreshold = 100;
        startingOffer = 20;
        currentOffer = 0;
        amountReceived = 0;
        status = NONE;
    }

    @IAgentActionFilter
    @IAgentActionName(name = "offer/send")
    public void sendOffer(Number targetVehicleId, Number bargainStatus){
        InstructionModel iOb;
        int offer = currentOffer;
        switch (bargainStatus.intValue()){
            case BARGAIN_START:{
                iOb = new InstructionModel(id, BargainConstants.SEND_OFFER);
                offer = startingOffer;
                status = OFFER_SENT;
                break;
            }
            case OFFER_ACCEPTED:{
                iOb = new InstructionModel(id, BargainConstants.SEND_PAYOUT);
                status = BARGAIN_END;
                break;
            }
            case OFFER_REJECTED:{
                iOb = new InstructionModel(id, BargainConstants.SEND_OFFER);
                offer += 50;
                break;
            }
            default:
                throw new RuntimeException("sendOffer: Unknown bargain status");
        }
        currentOffer = offer;

        iOb.pushInt(targetVehicleId.intValue());
        iOb.pushInt(offer);

        agentManager.addInstruction(iOb);
    }

    @IAgentActionFilter
    @IAgentActionName(name = "offer/evaluate")
    public void evaluateOffer(Number senderVehicleId, Number amount){
        InstructionModel iOb = new InstructionModel(id, BargainConstants.SEND_DECISION);
        iOb.pushInt(senderVehicleId.intValue());
        if(amount.intValue() < acceptThreshold)
            iOb.pushShort(OFFER_REJECTED);
        else
            iOb.pushShort(OFFER_ACCEPTED);
        agentManager.addInstruction(iOb);
    }

    @IAgentActionFilter
    @IAgentActionName(name = "offer/receive")
    public void receiveOfferPayout(Number senderVehicleId, Number payout){
        amountReceived = payout.intValue();
        System.out.println("Received " + amountReceived + "monetary units from agent " + senderVehicleId.intValue());
    }


    public static class CBargainAgentGenerator extends IBaseAgentGenerator<CBargainAgent> {

        private AgentManager agentManager;
        /**
         * @param p_stream ASL code as any stream e.g. FileInputStream
         * @param p_am Reference to agent manager
         */
        public CBargainAgentGenerator(@Nonnull final InputStream p_stream, AgentManager p_am) throws Exception
        {
            super(
                    // input ASL stream
                    p_stream,
                    Stream.concat(
                            CCommon.actionsFromPackage(),
                            CCommon.actionsFromAgentClass( CMarketAgent.class )
                    ).collect(Collectors.toSet()),
                    new CVariableBuilder()
            );
            agentManager = p_am;
        }

        @Nullable
        @Override
        public CBargainAgent generatesingle(@Nullable Object... p_data) {
            assert p_data != null;
            return new CBargainAgent(m_configuration, agentManager,(int)p_data[0],(String)p_data[1]);
        }
    }
}
