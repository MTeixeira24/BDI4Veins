package jasonveins.omnet.environment.dijkstra;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

public class Graph {
    private final Map<Integer, Vertex> vertexMap;
    private final List<Vertex> vertexes;
    private final List<Edge> edges;

    public Graph(List<Vertex> vertexes, List<Edge> edges) {
        this.vertexes = vertexes;
        this.edges = edges;
        vertexMap = null;
    }

    /**
     * Obtain a graph from a json file
     */
    public Graph(String filename){
        edges = new ArrayList<>();
        vertexes = new ArrayList<>();
        vertexMap = new HashMap<>();
        JSONParser parser = new JSONParser();

        try(FileReader fr = new FileReader(filename))
        {
            JSONObject jsonObject = (JSONObject)parser.parse(fr);

            Iterator it = jsonObject.keySet().iterator();
            //Get the nodes
            while (it.hasNext()){
                String key = (String)it.next();
                Vertex node = new Vertex(key, "N"+key);
                vertexMap.put(Integer.parseInt(key), node);
            }
            //Create the links
            it = jsonObject.keySet().iterator();
            while (it.hasNext()){
                String key = (String)it.next();
                //get the array
                JSONArray jar = (JSONArray)jsonObject.get(key);
                //Iterate over the array
                Iterator ait = jar.iterator();
                int counter = 0;
                while(ait.hasNext()){
                    Edge link = new Edge("Edge_" + ++counter, vertexMap.get(Integer.parseInt(key)), vertexMap.get(Integer.parseInt((String)ait.next())), 0);
                    edges.add(link);
                }
            }
            vertexes.addAll(vertexMap.values());

        } catch (ParseException e) {
            e.printStackTrace();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    public void updateWeights(int maxValue){
        for(Edge e : edges){
            e.updateSatisfactionCost(maxValue);
        }
    }

    public List<Vertex> getVertexes() {
        return vertexes;
    }

    public List<Edge> getEdges() {
        return edges;
    }

    public Map<Integer, Vertex> getVertexesMap(){
        return vertexMap;
    }

    public void alterRoute(LinkedList<Vertex> currentPath, int[] toRemove) {
        for(int i = 0; i < currentPath.size(); i++){
            Vertex v = currentPath.get(i);
            int id = Integer.parseInt(v.getId());
            if(Arrays.binarySearch(toRemove, id) >= 0){
                //Remove the edges leading to and from this vertex
                edges.removeIf(e -> e.getSource().equals(v) || e.getDestination().equals(v));
                //remove the vertex
                vertexMap.remove(id);
                vertexes.remove(v);
                return;
            }
        }
    }

    public LinkedList<Vertex> updateAndComputeShortestPath(List<Integer> p_newWeights){
        //Update node values
        for(int i = 0; i < getVertexes().size(); i++){
            getVertexes().get(i).setValue(p_newWeights.get(i));
        }
        //Update the graphs weights
        updateWeights(Collections.max(p_newWeights));
        //Get the route
        Dijkstra dijkstra = new Dijkstra(this);
        //Assume that the origin is at the beginning of the config file and the destination at the end of the config file
        dijkstra.execute(getVertexes().get(0));
        return dijkstra.getPath(getVertexes().get(getVertexes().size() - 1));
    }
}
