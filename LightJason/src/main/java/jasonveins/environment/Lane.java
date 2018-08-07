package jasonveins.environment;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicReferenceArray;

import org.lightjason.agentspeak.language.CLiteral;
import org.lightjason.agentspeak.language.CRawTerm;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.CTrigger;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.ITrigger;
import jasonveins.MyAgent;

import javax.annotation.Nonnull;

public class Lane {
    private final Link m_link;
    private final Node toNode;
    private final Node fromNode;
    private final int number;
    private final Map<MyAgent, Integer> laneBlockProgress = new HashMap<>();
    private final Map<MyAgent, Integer> vehiclesInLane = new ConcurrentHashMap<>();
    private final AtomicReferenceArray<MyAgent> positions;

    public Lane(Link p_link, Node p_toNode, Node p_fromNode, int p_number, int lane_length) {
        m_link = p_link;
        toNode = p_toNode;
        fromNode = p_fromNode;
        number = p_number;
        positions = new AtomicReferenceArray<>(lane_length);
    }

    public void insertVehicle(@Nonnull MyAgent vehicle){
        positions.compareAndSet(0, null, vehicle);
        vehiclesInLane.put(vehicle, 0);
        laneBlockProgress.put(vehicle, 0);
    }

    public void advanceVehicle(@Nonnull MyAgent vehicle){
        int newProgress = laneBlockProgress.get(vehicle) + vehicle.getSpeed();
        if(newProgress > 100){
            final int oldPosition = vehiclesInLane.get(vehicle);
            if(oldPosition < (positions.length() - 1) ){
                if(positions.compareAndSet(oldPosition + 1, null, vehicle)){
                    positions.set(oldPosition, null);
                    vehiclesInLane.put(vehicle, oldPosition + 1);
                    laneBlockProgress.put(vehicle, newProgress % 100);
                    final ITrigger trigger = CTrigger.from(
                            ITrigger.EType.ADDGOAL,
                            CLiteral.from(
                                    "lane/progress",
                                    CLiteral.from("in", CRawTerm.from(oldPosition + 1))
                            )//lane/progress(In(<NewPosition>))
                    );
                    vehicle.trigger(trigger);
                }else{

                }
            }
            else{
                positions.compareAndSet(oldPosition, vehicle, null);
                laneBlockProgress.remove(vehicle);
                vehiclesInLane.remove(vehicle);
                final ITrigger trigger = CTrigger.from(
                        ITrigger.EType.ADDGOAL,
                        CLiteral.from(
                                "lane/end"
                        )//lane/end()
                );
                vehicle.trigger(trigger);
            }
        }else{
            laneBlockProgress.put(vehicle, newProgress);
        }
    }

    public Link getLink() {
        return m_link;
    }

    public Node getToNode() {
        return toNode;
    }

    public Node getFromNode() {
        return fromNode;
    }

    public int getNumber() {
        return number;
    }
}
