package jasonveins.omnet;

import org.lightjason.agentspeak.agent.IAgent;
import org.lightjason.agentspeak.language.CLiteral;
import org.lightjason.agentspeak.language.CRawTerm;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.CTrigger;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.ITrigger;

import javax.annotation.Nonnull;
import java.io.FileInputStream;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.stream.Collectors;

public class AgentManager {
    protected Set<IAgent<?>> l_agents;
    protected String aslpath;
    protected AtomicBoolean execute, cycleEnd; //Flags to control the agent cycle. //
    // execute flag determines if new cycles should be made.
    // cycleEnd confirms if a cycle has properly finished in order to allow mutations to the agentSet
    protected final Map<Integer, ManagerAgent> agentMap = new ConcurrentHashMap<>();
    //Map agent Ids to their references.
    protected final ConnectionManager cmanager;
    protected final CopyOnWriteArrayList<String> instructionsList = new CopyOnWriteArrayList<>();

    public AgentManager(String m_aslpath, ConnectionManager m_cm) {
        cmanager = m_cm;
        aslpath = m_aslpath;
        execute = new AtomicBoolean(true);
        cycleEnd = new AtomicBoolean(true);
        try
                (
                        final FileInputStream l_stream = new FileInputStream( aslpath )
                )
        {
            final int l_agentNumber = 1;
            l_agents =
                    new ManagerGenerator( l_stream, this )
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

    public void createNewAgent(int id){
        try
                (
                        final FileInputStream l_stream = new FileInputStream( aslpath )
                ){
            if(!execute.compareAndSet(true, false)){ //Halt execution of the agent loop
                throw new RuntimeException();
            }
            while(!cycleEnd.get()); //Wait until current stream is over
            ManagerAgent m_ag = new ManagerGenerator(l_stream, this).generatesingle(id);
            l_agents.add(m_ag);
            agentMap.putIfAbsent(id, m_ag);
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
        ManagerAgent vehicle = agentMap.get(id);
        if(vehicle == null) throw new RuntimeException();
        vehicle.trigger(trigger);
    }

    public void addInstruction(@Nonnull String data){
        instructionsList.add(data);
    }

    public CopyOnWriteArrayList<String> extractInstructions(){
        @SuppressWarnings("unchecked")
        CopyOnWriteArrayList<String> inst = ((CopyOnWriteArrayList<String>) instructionsList.clone());
        instructionsList.removeAll(inst);
        return inst;
    }

    public boolean existsInstructions(){
        return !instructionsList.isEmpty();
    }

    public void loop(){
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
            }

        }
    }

    public Set<IAgent<?>> agents(){
        return this.l_agents;
    }
}
