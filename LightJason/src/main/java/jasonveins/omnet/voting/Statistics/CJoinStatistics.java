package jasonveins.omnet.voting.Statistics;

import java.io.File;
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Map;

import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;


import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.apache.commons.text.similarity.HammingDistance;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.xml.sax.SAXException;

public class CJoinStatistics extends IStatistics {
    private int initPlatoonSpeed;
    private int finalPlatoonSpeed;
    private ConcurrentHashMap<Integer, ArrayList<Double>> initial_final_utilities;
    private boolean rejected;

    public CJoinStatistics(){
        initial_final_utilities = new ConcurrentHashMap<>();
        rejected = false;
    }

    public void dumpCSV(){

        try {
            FileWriter out = null;
            File f = new File("testResults/JoinResults.csv");
            if(!f.exists() || f.isDirectory()) {
                out = new FileWriter(f);
                out.write("Scenario,Rule,CommitteeRule,PlatoonSize,Iteration,Rejected,Agent,InitialUtility,FinalUtility,Difference,HammingDistance\n");
            }else{
                out = new FileWriter(f, true);
            }
            String head = type+","+rule+","+committeeRule+","+platoonSize+","+iteration+',';
            if(rejected){
                out.write(head+"1,,,,,\n");
            }else{
                head = head + "0,";
                Iterator<Map.Entry<Integer, ArrayList<Double>>> it = initial_final_utilities.entrySet().iterator();
                for(int i = 0; i < initial_final_utilities.entrySet().size() - 1; i++){
                    Map.Entry<Integer, ArrayList<Double>> e = it.next();
                    double diff = e.getValue().get(1) - e.getValue().get(0);
                    out.write(head+e.getKey()+","+e.getValue().get(0)+","+e.getValue().get(1)+","+diff+","+e.getValue().get(2)+"\n");
                }
            }
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void setInitPlatoonSpeed(int initPlatoonSpeed) {
        this.initPlatoonSpeed = initPlatoonSpeed;
    }

    public void setFinalPlatoonSpeed(int finalPlatoonSpeed) {
        this.finalPlatoonSpeed = finalPlatoonSpeed;
    }

    public void setInitAndFinalUtil(int id, double init, double fin, double hamming){
        ArrayList<Double> utils = new ArrayList<>(3);
        utils.add(init);
        utils.add(fin);
        utils.add(hamming);
        initial_final_utilities.put(id, utils);
    }

    public void setRejected(boolean val){
        rejected = val;
    }
}
