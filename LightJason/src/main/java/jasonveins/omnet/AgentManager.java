package jasonveins.omnet;

import jasonveins.omnet.decision.DecisionDataModel;
import jasonveins.omnet.decision.InstructionModel;
import org.lightjason.agentspeak.agent.IAgent;
import org.lightjason.agentspeak.language.CLiteral;
import org.lightjason.agentspeak.language.CRawTerm;
import org.lightjason.agentspeak.language.ITerm;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.CTrigger;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.ITrigger;

import javax.annotation.Nonnull;
import java.io.FileInputStream;
import java.nio.ByteBuffer;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.Predicate;
import java.util.stream.Collectors;

public class AgentManager {
    protected Set<IAgent<?>> l_agents;
    protected String aslpath;
    protected AtomicInteger agentCount = new AtomicInteger(0);
    protected AtomicBoolean execute, cycleEnd, simulate; //Flags to control the agent cycle. //
    // execute flag determines if new cycles should be made.
    // cycleEnd confirms if a cycle has properly finished in order to allow mutations to the agentSet
    protected final Map<Integer, NormalVehicleAgent> agentMap = new ConcurrentHashMap<>();
    //Map agent Ids to their references.
    protected final ConnectionManager cmanager;
    //protected final CopyOnWriteArrayList<String> instructionsList = new CopyOnWriteArrayList<>();
    protected final DecisionDataModel ddm = new DecisionDataModel();

    public AgentManager(String m_aslpath, ConnectionManager m_cm) {
        cmanager = m_cm;
        aslpath = m_aslpath;
        execute = new AtomicBoolean(true);
        cycleEnd = new AtomicBoolean(true);
        simulate = new AtomicBoolean(false);
        try
                (
                        final FileInputStream l_stream = new FileInputStream( aslpath ) //TODO: Get rid of this
                )
        {
            final int l_agentNumber = 0;
            l_agents =
                    new NormalVehicleGenerator( l_stream, this )
                            .generatemultiple( l_agentNumber, 0 )
                            .collect( Collectors.toSet()

            );
        }
        catch ( final Exception l_exception )
        {
            l_exception.printStackTrace();
            return;
        }
    }

    public void createNewAgent(int id, String vType, String aslFile){
        try
                (
                        final FileInputStream l_stream = new FileInputStream( aslFile )
                ){
            if(!execute.compareAndSet(true, false)){ //Halt execution of the agent loop
                throw new RuntimeException();
            }
            while(!cycleEnd.get()); //Wait until current stream is over
            NormalVehicleAgent m_ag;
            switch(vType){
                default:
                    m_ag = new NormalVehicleGenerator(l_stream, this).generatesingle(id, vType); //TODO: Make the generators global
                    break;
            }

            l_agents.add(m_ag);
            agentMap.putIfAbsent(id, m_ag);
            agentCount.incrementAndGet();
            if(!execute.compareAndSet(false, true)){ //Restart execution of the agent loop
                throw new RuntimeException();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void updateBeliefs(@Nonnull Integer id, @Nonnull String belief, @Nonnull String value){
        final ITrigger trigger = CTrigger.from(
                ITrigger.EType.ADDBELIEF,
                CLiteral.from(
                        belief,
                        CRawTerm.from(Double.valueOf(value))
                )//belief(<value>)
        );
        NormalVehicleAgent vehicle = agentMap.get(id);
        if(vehicle == null) throw new RuntimeException();
        vehicle.trigger(trigger);
    }

    public void updateBeliefs(@Nonnull int id, @Nonnull String belief,@Nonnull ByteBuffer values){
        ArrayList<CRawTerm<?>> terms = new ArrayList<>();
        short data_type = values.getShort();
        switch (data_type){
            case Constants.VALUE_BOOL:
                throw new RuntimeException("Bool value not defined!");
            case Constants.VALUE_CHAR:
                terms.add(CRawTerm.from(values.getChar()));
                break;
            case Constants.VALUE_SHORT:
                terms.add(CRawTerm.from(values.getShort()));
                break;
            case Constants.VALUE_INT:
                terms.add(CRawTerm.from(values.getInt()));
                break;
            case Constants.VALUE_LONG:
                terms.add(CRawTerm.from(values.getLong()));
                break;
            case Constants.VALUE_FLOAT:
                terms.add(CRawTerm.from(values.getFloat()));
                break;
            case Constants.VALUE_DOUBLE:
                terms.add(CRawTerm.from(values.getDouble()));
                break;
            default:
                throw new RuntimeException("Unknown value!");
        }

        /*final ITrigger trigger = CTrigger.from(
                ITrigger.EType.ADDBELIEF,
                CLiteral.from(belief,
                        Arrays.stream( (ITerm[])terms.toArray() ).collect( Collectors.toList() )
                )
        );*/
        ITrigger trigger = null;
        //java.lang.ClassCastException: java.base/[Ljava.lang.Object; cannot be cast to [Lorg.lightjason.agentspeak.language.ITerm;
        try{
            ITerm[] arrayTerms = new ITerm[terms.size()];
            arrayTerms = terms.toArray(arrayTerms);
            trigger = CTrigger.from(
                    ITrigger.EType.ADDBELIEF,
                    CLiteral.from(belief,
                            Arrays.stream( arrayTerms ).collect( Collectors.toList() )
                    )
            );
        }catch (Exception e){
            e.printStackTrace();
        }
        NormalVehicleAgent vehicle = agentMap.get(id);
        if(vehicle == null) throw new RuntimeException();
        vehicle.trigger(trigger);
    }

    public void addInstruction(@Nonnull InstructionModel data){
        ddm.addInstruction(data);
    }

    public byte[] extractInstructions(){
        return ddm.convertToMessage();
    }

    public boolean existsInstructions(){
        return !ddm.isEmpty();
    }

    public void loop(){
        System.out.println("Agent loop: Awaiting agent input");
        while(!simulate.get() && !cmanager.isDisconnected()); //TODO: Use sleep instead of a while
        if(cmanager.isDisconnected()){
            //Abrupt connection interruption
            System.exit(1);
        }
        System.out.println("Agent loop: Starting agent loop");
        while(true){
            if(execute.get()){
                cycleEnd.compareAndSet(true, false);
                l_agents.parallelStream()
                        .forEach( i ->
                        {
                            try
                            {
                                i.call();
                            }
                            catch ( final Exception l_exception )
                            {
                                l_exception.printStackTrace();
                                throw new RuntimeException();
                            }
                        } );
                cycleEnd.compareAndSet(false, true);
                if(!simulate.get()) break;
            }
        }
        cmanager.finish();
    }

    public Set<IAgent<?>> agents(){
        return this.l_agents;
    }

    public void removeAgent(int agentId) {
        if(agentMap.get(agentId) != null && agentMap.size() > 0){
            if(!execute.compareAndSet(true, false)){ //Halt execution of the agent loop
                throw new RuntimeException();
            }
            while(!cycleEnd.get()); //Wait until current stream is over
            Predicate<IAgent<?>> removePredicate = a -> ((NormalVehicleAgent)a).id() == agentId;
            l_agents.removeIf(removePredicate);
            agentMap.remove(agentId);
            if(agentMap.size() == 0){
                System.out.println("No more agents in simulation. Terminating");
                toggleAgentLoop(true, false);
            }
            if(!execute.compareAndSet(false, true)){ //Restart execution of the agent loop
                throw new RuntimeException();
            }
        }else{
            throw new RuntimeException();
        }

    }

    public void toggleAgentLoop(boolean expectedValue, boolean newValue) {
        if( !simulate.compareAndSet(expectedValue, newValue)) throw new RuntimeException();
    }
}
