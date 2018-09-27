package jasonveins.omnet.voting;

import java.io.File;
import java.util.ArrayList;
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

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.xml.sax.SAXException;

public class CStatistics {
    private int initPlatoonSpeed;
    private int finalPlatoonSpeed;
    private ConcurrentHashMap<Integer, ArrayList<Double>> initial_final_utilities;
    private boolean rejected;

    private int platoonSize;
    private String type;
    private String rule;
    private int iteration;

    public CStatistics(){
        initial_final_utilities = new ConcurrentHashMap<>();
        rejected = false;
    }

    public void dump(){
        try {
            DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
            DocumentBuilder docBuilder = docFactory.newDocumentBuilder();
            Document doc;
            Element rootElement;

            File f = new File("testResults/" + type + rule + platoonSize + ".xml");
            if(f.exists() && !f.isDirectory()) {
                //get existing xml data
                doc = docBuilder.parse(f);
                rootElement = doc.getDocumentElement();
            }else{
                //create new document and set the root elements
                doc = docBuilder.newDocument();
                rootElement = doc.createElement("Simulation");
                doc.appendChild(rootElement);
            }
            // Data for each iteration
            Element iterElement = doc.createElement("Iteration");
            rootElement.appendChild(iterElement);

            // Iteration Attribute
            iterElement.setAttribute("number", String.valueOf(iteration));

            //Platoon data node
            Element platoonData = doc.createElement("Platoon");
            platoonData.setAttribute("initialSpeed", String.valueOf(initPlatoonSpeed));
            platoonData.setAttribute("finalSpeed", String.valueOf(finalPlatoonSpeed));
            iterElement.appendChild(platoonData);

            //Rejection data
            Element rejectedData = doc.createElement("Rejected");
            iterElement.appendChild(rejectedData);
            if(rejected)
                rejectedData.appendChild(doc.createTextNode("yes"));
            else{
                rejectedData.appendChild(doc.createTextNode("no"));

                for(Map.Entry<Integer, ArrayList<Double>> e : initial_final_utilities.entrySet()){

                    //Agent data
                    Element agentData = doc.createElement("Agent");
                    agentData.setAttribute("id", String.valueOf(e.getKey()));

                    Element initialUtility = doc.createElement("initialUtility");
                    initialUtility.appendChild(doc.createTextNode(String.valueOf(e.getValue().get(0))));
                    agentData.appendChild(initialUtility);

                    Element finalUtility = doc.createElement("finalUtility");
                    finalUtility.appendChild(doc.createTextNode(String.valueOf(e.getValue().get(1))));
                    agentData.appendChild(finalUtility);

                    iterElement.appendChild(agentData);
                }
            }

            // write the content into xml file
            TransformerFactory transformerFactory = TransformerFactory.newInstance();
            Transformer transformer = transformerFactory.newTransformer();
            //transformer.setOutputProperty(OutputKeys.INDENT, "yes");
            DOMSource source = new DOMSource(doc);
            StreamResult result = new StreamResult(f);

            // Output to console for testing
            // StreamResult result = new StreamResult(System.out);

            transformer.transform(source, result);



        } catch (ParserConfigurationException pce) {
            pce.printStackTrace();
        } catch (TransformerException tfe) {
            tfe.printStackTrace();
        } catch (SAXException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println("XML dump complete");
    }

    public void setInitPlatoonSpeed(int initPlatoonSpeed) {
        this.initPlatoonSpeed = initPlatoonSpeed;
    }

    public void setFinalPlatoonSpeed(int finalPlatoonSpeed) {
        this.finalPlatoonSpeed = finalPlatoonSpeed;
    }

    public void setInitAndFinalUtil(int id, double init, double fin){
        ArrayList<Double> utils = new ArrayList<>(2);
        utils.add(init);
        utils.add(fin);
        initial_final_utilities.put(id, utils);
    }

    public void setRejected(boolean val){
        rejected = val;
    }

    public void setSimParams(int platoonSize, int iteration, String rule, String type){
        this.platoonSize = platoonSize;
        this.rule = rule;
        this.type = type;
        this.iteration = iteration;
    }
}
