package jasonveins.omnet.agent.utilityFunctions;

import jasonveins.omnet.environment.dijkstra.Graph;

import java.util.List;

public interface IUtilityFunction {

    double computeUtilitySpeed(double val);
    double computeUtilityRoute(List<Integer> route, Graph routeGraph);
    int getHammingDistance(List<Integer> evalArray, Graph route);
    int getHammingDistance();

    void setPreferredSpeed(int speed);
    void setPreferredRoute(List<Integer> route);

    int getPreferredSpeed();
    List<Integer> getPreferredRoute();
}
