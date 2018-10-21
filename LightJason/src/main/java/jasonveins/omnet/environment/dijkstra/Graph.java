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
}