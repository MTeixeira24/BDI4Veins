package jasonveins.omnet.managers;

import jasonveins.omnet.agent.NormalVehicleAgent;
import jasonveins.omnet.agent.NormalVehicleGenerator;
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
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.Predicate;
import java.util.stream.Collectors;

/**
 * Manages the runtime environment of the agents and proxies calls between the agents and the omnet nodes
 */
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
    //Used to wait for initial agent creation. Connection manager holds a reference to this and decrements when the first
    //agent is created
    private CountDownLatch latch = new CountDownLatch(1);

    /**
     * Class constructor
     * @param m_aslpath Path to asl files
     * @param m_cm Reference to the connection manager
     */
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

    /**
     * Create a new agent and insert it into the runtime. The agent loop is safely paused before insertion
     * @param id Identifier to be assigned to the agent. This value is determined in the omnet side and should be equal to its corresponding OMNET node
     * @param vType Vehicle type as specified in a SUMO routes configuration file
     * @param aslFile The asl file to be used by the agent
     */
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

    /**
     * Value agnostic belief update method. Does not support nested belief values.
     * @param id Identifier of the agent
     * @param belief Belief to add to the agent
     * @param values Byte sequence of the values within the belief as received from the connection manager
     */
    public void updateBeliefs(int id, @Nonnull String belief,@Nonnull ByteBuffer values){
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

        ITrigger trigger = null;
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

    /**
     * Pushes unto the decision data model the decisions an agent wished to transmit to its OMNET nodes.
     * @param data The data to be sent as an Instruction Model object
     */
    public void addInstruction(@Nonnull InstructionModel data){
        ddm.addInstruction(data);
    }

    /**
     * Request the Decision Data Model to send all the agent decisions its holding in byte sequence form. WARNING: All currently held data at time of method call will be deleted
     * @return Byte sequence of all decisions made by agent ready to be sent over to OMNET
     */
    public byte[] extractInstructions(){
        return ddm.convertToMessage();
    }

    /**
     * Checks if any instructions are available in the Decision Data Model
     * @return True if the number of instructions is greater than zero, false otherwise
     */
    public boolean existsInstructions(){
        return !ddm.isEmpty();
    }

    public void loop(){
        System.out.println("Agent loop: Awaiting agent input");

        try {
            latch.await();  // wait until latch counted down to 0
            } catch (InterruptedException e) {
                e.printStackTrace();
         }
         toggleAgentLoop(false, true);


        System.out.println("Resume");

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

    public CountDownLatch getLatch() { return this.latch; }
}
