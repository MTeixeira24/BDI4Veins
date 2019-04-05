package jasonveins.omnet.agent.voting;

import jasonveins.omnet.environment.dijkstra.Graph;
import jasonveins.omnet.environment.dijkstra.Vertex;

import java.util.ArrayList;
import java.util.LinkedList;

public class CVoteParameters {
    private double utilityFactor;
    private int preferredSpeed;
    private Graph route;
    private int platoonSpeed;
    private LinkedList<Vertex> currentPath;
    private ArrayList<Integer> members;

    CVoteParameters(double utilityFactor, int preferredSpeed, Graph route, int platoonSpeed,
                    LinkedList<Vertex> currentPath, ArrayList<Integer> members){
        this.utilityFactor = utilityFactor;
        this.platoonSpeed = platoonSpeed;
        this.preferredSpeed = preferredSpeed;
        this.route = route;
        this.currentPath = currentPath;
        this.members = members;
    }

    CVoteParameters(double utilityFactor, int preferredSpeed, Graph route, int platoonSpeed,
                    ArrayList<Integer> members){
        this(utilityFactor, preferredSpeed,route, platoonSpeed, null, members);
    }

    CVoteParameters(double utilityFactor, int preferredSpeed, int platoonSpeed, ArrayList<Integer> members){
        this(utilityFactor, preferredSpeed, null, platoonSpeed, null, members);
    }

    CVoteParameters(){
        this(0,100,null,100,null,null);
    }

    public double getUtilityFactor() {
        return utilityFactor;
    }

    public void setUtilityFactor(double utilityFactor) {
        this.utilityFactor = utilityFactor;
    }

    public int getPreferredSpeed() {
        return preferredSpeed;
    }

    public void setPreferredSpeed(int preferredSpeed) {
        this.preferredSpeed = preferredSpeed;
    }

    public Graph getRoute() {
        return route;
    }

    public void setRoute(Graph route) {
        this.route = route;
    }

    public int getPlatoonSpeed() {
        return platoonSpeed;
    }

    public void setPlatoonSpeed(int platoonSpeed) {
        this.platoonSpeed = platoonSpeed;
    }

    public LinkedList<Vertex> getCurrentPath() {
        return currentPath;
    }

    public void setCurrentPath(LinkedList<Vertex> currentPath) {
        this.currentPath = currentPath;
    }

    public ArrayList<Integer> getMembers() {
        return members;
    }

    public void setMembers(ArrayList<Integer> members) {
        this.members = members;
    }
}
