package jasonveins.omnet.agent.utilityFunctions;

import jasonveins.omnet.environment.dijkstra.Graph;

import java.util.*;

public class CGaussianUtility implements IUtilityFunction {

    private int committeeTolerance;
    private int speedTolerance;
    private List <Integer> preferredRoute;
    private int preferredSpeed;

    public CGaussianUtility(){
        this(300, 7);
    }

    public CGaussianUtility(int speedTolerance, int committeeTolerance){
        this.speedTolerance = speedTolerance;
        this.committeeTolerance = committeeTolerance;
    }

    @Override
    public double computeUtilitySpeed(double val) {
        double exponent = -1*(Math.pow((val - preferredSpeed),2) / speedTolerance);
        return Math.pow(Math.E, exponent);
    }

    @Override
    public double computeUtilityRoute(List<Integer> route, Graph routeGraph) {
        int inverseHammming = getHammingDistance(route, routeGraph);
        int maxHamming = route.size();
        double exponent = -1*(Math.pow((inverseHammming - maxHamming),2) / committeeTolerance);
        return Math.pow(Math.E, exponent);
    }

    @Override
    public int getHammingDistance(List<Integer> evalArray, Graph route) {
        ArrayList<Integer> preferredVector = new ArrayList<>(Collections.nCopies(evalArray.size(), 0));
        Set<Integer> vertexes = route.getVertexesMap().keySet();
        Iterator<Integer> it = vertexes.iterator();
        int position = 0;
        while(it.hasNext()){
            int node = it.next();
            if(preferredRoute.contains(node))
                preferredVector.set(position, 1);
            position++;
        }
        /*
        [1,1,0,0,1,1,1]
        [2,4,7,8]
        [0,0,0,0,0,0,0]
         */
        //get hamming distance, inverted in order to work with preexisting plans
        int hammingDistance = evalArray.size();
        for(int i = 0; i < evalArray.size(); i++){
            if(!evalArray.get(i).equals(preferredVector.get(i))) hammingDistance--;
        }
        //this.hammingDistance = hammingDistance;
        return hammingDistance;
    }

    @Override
    public void setPreferredSpeed(int speed) {
        preferredSpeed = speed;
    }

    @Override
    public void setPreferredRoute(List<Integer> route) {
        preferredRoute = route;
    }
}
