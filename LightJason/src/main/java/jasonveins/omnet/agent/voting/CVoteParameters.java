package jasonveins.omnet.agent.voting;

import jasonveins.omnet.environment.dijkstra.Graph;
import jasonveins.omnet.environment.dijkstra.Vertex;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

public class CVoteParameters {
    private int preferredSpeed;
    private Graph route;
    private int platoonSpeed;
    private LinkedList<Vertex> currentPath;
    private List<Integer> members;
    private List<Integer> preferredPath;
    private int isLeader;

    CVoteParameters(int preferredSpeed, Graph route, int platoonSpeed, int isLeader,
                    LinkedList<Vertex> currentPath, List<Integer> members,
                    List<Integer> preferredPath){
        this.platoonSpeed = platoonSpeed;
        this.preferredSpeed = preferredSpeed;
        this.route = route;
        this.currentPath = currentPath;
        this.members = members;
        this.isLeader = isLeader;
        this.setPreferredPath(preferredPath);
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

    public List<Integer> getMembers() {
        return members;
    }

    public void setMembers(List<Integer> members) {
        this.members = members;
    }

    List<Integer> getPreferredPath() {
        return preferredPath;
    }

    void setPreferredPath(List<Integer> preferredPath) {
        this.preferredPath = preferredPath;
    }
}
