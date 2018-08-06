package trafficproject.environment;

import javax.annotation.Nonnull;
import java.util.Vector;

public class Link {
    private Node[] connectingNodes = new Node[2];
    private Vector<Lane> lanes = new Vector<>();

    public Link(@Nonnull final Node nodeA, @Nonnull final Node nodeB, @Nonnull final int lanesAB,
         @Nonnull final int lanesBA, @Nonnull final int laneSize){
        connectingNodes[0] = nodeA;
        connectingNodes[1] = nodeB;
        for(int i = 0; i < lanesAB; i++){
            lanes.add(new Lane(this, nodeB, nodeA, i, laneSize));
        }
        for(int i = 0; i < lanesBA; i++){
            lanes.add(new Lane(this, nodeA, nodeB, i, laneSize));
        }
    }

    public final Node[] nodes(){
        return connectingNodes;
    }

    public final Vector<Lane> lanes(){
        return lanes;
    }
}
