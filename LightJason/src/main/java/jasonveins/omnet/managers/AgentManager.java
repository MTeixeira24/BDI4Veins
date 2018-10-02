package jasonveins.omnet.managers;

import jasonveins.omnet.agent.CVoterAgentGenerator;
import jasonveins.omnet.agent.IVehicleAgent;
import jasonveins.omnet.agent.NormalVehicleAgent;
import jasonveins.omnet.agent.NormalVehicleGenerator;
import jasonveins.omnet.decision.DecisionDataModel;
import jasonveins.omnet.decision.InstructionModel;
import jasonveins.omnet.voting.CStatistics;
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

public class AgentManager extends Thread {
    protected static final String resourceFolder = "src/main/resources/asl/";
    protected Set<IAgent<?>> l_agents;
    protected String aslpath;
    protected AtomicInteger agentCount = new AtomicInteger(0);
    protected AtomicBoolean execute, cycleEnd, simulate; //Flags to control the agent cycle. //
    // execute flag determines if new cycles should be made.
    // cycleEnd confirms if a cycle has properly finished in order to allow mutations to the agentSet
    protected final Map<Integer, IVehicleAgent<?>> agentMap = new ConcurrentHashMap<>();
    //Map agent Ids to their references.
    protected final ConnectionManager cmanager;
    //protected final CopyOnWriteArrayList<String> instructionsList = new CopyOnWriteArrayList<>();
    protected final DecisionDataModel ddm = new DecisionDataModel();
    //Used to wait for initial agent creation. Connection manager holds a reference to this and decrements when the first
    //agent is created
    private CountDownLatch latch = new CountDownLatch(1);

    private CStatistics stats;
    private int debugger = 0;

    /**
     * Class constructor
     * @param m_aslpath Path to asl files
     * @param m_cm Reference to the connection manager
     */
    public AgentManager(String m_aslpath, ConnectionManager m_cm) {
        stats = new CStatistics();
        cmanager = m_cm;
        aslpath = m_aslpath;
        execute = new AtomicBoolean(true);
        cycleEnd = new AtomicBoolean(true);
        simulate = new AtomicBoolean(false);
        String l_aslpath = resourceFolder+aslpath;
        try
                (
                        final FileInputStream l_stream = new FileInputStream( l_aslpath ) //TODO: Get rid of this
                )
        {
            final int l_agentNumber = 0;
            l_agents =
                    new CVoterAgentGenerator( l_stream, this )
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

    @Override
    public void run(){
        this.loop();
    }

    /**
     * Create a new agent and insert it into the runtime. The agent loop is safely paused before insertion
     * @param id Identifier to be assigned to the agent. This value is determined in the omnet side and should be equal to its corresponding OMNET node
     * @param vType Vehicle type as specified in a SUMO routes configuration file
     * @param p_aslFile The asl file to be used by the agent
     */
    public void createNewAgent(int id, String vType, String p_aslFile){
        String l_aslFile = resourceFolder + p_aslFile;
        try
                (
                        final FileInputStream l_stream = new FileInputStream( l_aslFile )
                ){
            if(!execute.compareAndSet(true, false)){ //Halt execution of the agent loop
                throw new RuntimeException();
            }
            while(!cycleEnd.get()); //Wait until current stream is over
            /*NormalVehicleAgent m_ag;
            switch(vType){
                default:
                    m_ag = new NormalVehicleGenerator(l_stream, this).generatesingle(id, vType);
                    break;
            }*/

            IVehicleAgent<?> m_ag = buildAgent(id, l_stream, l_aslFile, vType);

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
     * Method called by createNewAgent to build an agent to add to the runtime. It is recommended to override this
     * method if new vehicle to behaviour associations are to be made.
     * @param p_id Id of the agent. Generated in OMNET and is equal to its corresponding control application
     * @param p_stream Input stream for the asl file
     * @param p_aslFile Name of the asl file. Must exist in folder: jasonveins.resources.asl
     * @param vType The vehicle type identifier as defined in SUMO
     * @return A vehicle agent object that subclasses IVehicleAgent
     */
    public IVehicleAgent<?> buildAgent(int p_id, @Nonnull FileInputStream p_stream, @Nonnull String p_aslFile, @Nonnull String vType){
        NormalVehicleAgent l_ag = null;
        try {
            switch(vType){
                case "vVoter":
                    if(p_aslFile.equals("FuelVoter.asl") || p_aslFile.equals("SpeedVoter.asl")){

                    }else{
                        throw new RuntimeException("Invalid asl file specified for vehicle type " + vType +". Got " + p_aslFile + "expected FuelVoter.asl or SpeedVoter.asl");
                    }
                    break;
                default:
                    l_ag = new NormalVehicleGenerator(p_stream, this).generatesingle(p_id, vType); //TODO: Make the generators global
                    break;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return l_ag;
    }

    /**
     * Value agnostic goal insertion method. Does not support nested predicates values.
     * @param id Identifier of the agent
     * @param belief Belief to add to the agent
     * @param values Byte sequence of the values within the belief as received from the connection manager
     * @param p_size Size in bytes of the message
     */
    public void updateGoals(int id, @Nonnull String belief,@Nonnull ByteBuffer values, int p_size){
        ArrayList<CRawTerm<?>> terms = new ArrayList<>();
        /**
         * From the debug below, it is guaranteed that this is not the issue
         * The agent themselves can receive the beliefs just fine.
         * Problem may be in omnet transmission, socket or trigger overload.
         * e.g.
         *Reset debug counter: 0
         * Agent   0    got notification to vote on a join:    [80, 85, 90, 95, 100, 105, 110, 115, 120]
         * [0, 0, 1, 1, 1, 1, 1, 1, 1]
         * Agent   0   Im the chair so no need to pass through omnet
         * Got vote   [0, 0, 1, 1, 1, 1, 1, 1, 1]
         * Agent   1    got notification to vote on a join:    [80, 85, 90, 95, 100, 105, 110, 115, 120]
         * [0, 0, 1, 1, 1, 1, 1, 1, 0]
         * Agent   1   Sending the vote down omnet
         * LIGHTJASON MANAGER: Got vote number: 1
         * Got vote   [0, 0, 1, 1, 1, 1, 1, 1, 0]
         * No more agents in simulation. Terminating
         */
        /*if(belief.equals("handle/submit/vote")){
            debugger++;
            System.out.println("LIGHTJASON MANAGER: Got vote number: " + debugger);
        }
        if(belief.equals("maneuver/complete")){
            debugger = 0;
            System.out.println("Reset debug counter: " + debugger);
        }*/
        int size = p_size;
        while(size > 0){
            short data_type = values.getShort();
            switch (data_type){
                case Constants.VALUE_BOOL:
                    throw new RuntimeException("Bool value not defined!");
                case Constants.VALUE_CHAR:
                    terms.add(CRawTerm.from(values.getChar()));
                    size -= 1;
                    break;
                case Constants.VALUE_SHORT:
                    terms.add(CRawTerm.from(values.getShort()));
                    size -= 2;
                    break;
                case Constants.VALUE_INT:
                    terms.add(CRawTerm.from(values.getInt()));
                    size -= 4;
                    break;
                case Constants.VALUE_LONG:
                    terms.add(CRawTerm.from(values.getLong()));
                    size -= 8;
                    break;
                case Constants.VALUE_FLOAT:
                    terms.add(CRawTerm.from(values.getFloat()));
                    size -= 4;
                    break;
                case Constants.VALUE_DOUBLE:
                    terms.add(CRawTerm.from(values.getDouble()));
                    size -= 8;
                    break;
                case Constants.VALUE_ARRAY:
                    int arraySize;
                    switch (values.getInt()){ //advance past the type specifier
                        case Constants.VALUE_INT:{
                            arraySize = values.getInt();
                            ArrayList<Integer> elements = new ArrayList<>(arraySize);
                            for(int i = 0; i < arraySize; i++){
                                elements.add(values.getInt());
                            }
                            terms.add(CRawTerm.from(elements));
                            size -= 4*(arraySize + 2);
                            break;
                        }
                        case Constants.VALUE_DOUBLE:{
                            arraySize = values.getInt();
                            ArrayList<Double> elements = new ArrayList<>(arraySize);
                            for(int i = 0; i < arraySize; i++){
                                elements.add(values.getDouble());
                            }
                            terms.add(CRawTerm.from(elements));
                            size -= 8*(arraySize + 1);
                            break;
                        }
                        default:
                            throw new RuntimeException("AgentManager: Unknown array value!");
                    }
                    break;
                default:
                    throw new RuntimeException("Unknown value!");
            }
        }

        ITrigger trigger = null;
        try{
            ITerm[] arrayTerms = new ITerm[terms.size()];
            arrayTerms = terms.toArray(arrayTerms);
            trigger = CTrigger.from(
                    ITrigger.EType.ADDGOAL,
                    CLiteral.from(belief,
                            Arrays.stream( arrayTerms ).collect( Collectors.toList() )
                    )
            );
        }catch (Exception e){
            e.printStackTrace();
        }
        IVehicleAgent<?> vehicle = agentMap.get(id);
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
        System.out.println("#################################");
        System.out.println("Agent loop: Awaiting agent input");

        try {
            latch.await();  // wait until latch counted down to 0
            } catch (InterruptedException e) {
                e.printStackTrace();
         }
         toggleAgentLoop(false, true);


        System.out.println("Resume. Iteration number: " + stats.getIteration());

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
        stats.dumpCSV();
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
            Predicate<IAgent<?>> removePredicate = a -> ((IVehicleAgent)a).id() == agentId;
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

    public boolean getAgentLoopStatus(){
        return simulate.get();
    }

    public CountDownLatch getLatch() { return this.latch; }

    public CStatistics getStats(){
        return stats;
    }

    public void setSimParams(int iteration, int run, String rule, String type) {
        return;
    }
}
