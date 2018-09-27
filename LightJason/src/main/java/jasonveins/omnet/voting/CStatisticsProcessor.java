package jasonveins.omnet.voting;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.*;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.w3c.dom.*;
import org.xml.sax.SAXException;

final class CStatisticsProcessor {

    public static void main(final String[] p_args){
        //DataProcessor dp = new DataProcessor();
        String[] fileNames = {"Unstable", "Stable"};
        String[] voteRules = {"Borda", "Approval", "Copeland", "Plurality"};
        int[] platoonSize = {4,5,6,7,8};

        try {

            for(int i = 0; i < fileNames.length; i++){
                int numRejections = 0;
                int counter = 0;
                DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
                DocumentBuilder docBuilder = docFactory.newDocumentBuilder();
                Document doc = docBuilder.newDocument();
                //File to store information
                File f = new File("testResults/" + fileNames[i] + ".xml");
                //Root of file
                Element rootElement = doc.createElement("Results");
                doc.appendChild(rootElement);
                //Iterate over all the voting rules
                for(int j = 0; j < voteRules.length; j++){
                    //Create a node named after the rule
                    Element ruleElement = doc.createElement(voteRules[j]);
                    rootElement.appendChild(ruleElement);
                    //Iterate over every file
                    for(int z = 0; z < platoonSize.length; z++){
                        double maxDif = -1;
                        double minDif = Double.MAX_VALUE;
                        double average = 0;
                        double median = 0;
                        ArrayList<Double> diffList = new ArrayList<>();
                        Element sizeElement = doc.createElement("Size");
                        sizeElement.setAttribute("value", String.valueOf(platoonSize[z]));
                        ruleElement.appendChild(sizeElement);
                        //Get the file
                        File p = new File("testResults/"+fileNames[i]+"/"+fileNames[i]+voteRules[j]+platoonSize[z]+".xml");
                        Document parseDoc = docBuilder.parse(p);
                        Element trootElement = parseDoc.getDocumentElement();
                        trootElement.normalize();
                        NodeList iterations = trootElement.getElementsByTagName("Iteration");
                        for(int iter = 0; iter < iterations.getLength(); iter++){
                            Node nNode = iterations.item(iter);
                            Node pNode = nNode.getChildNodes().item(0);
                            int finalPlatoonSpeed = Integer.valueOf(pNode.getAttributes().getNamedItem("finalSpeed").getTextContent());
                            String rejection = nNode.getChildNodes().item(1).getTextContent();
                            if(finalPlatoonSpeed == 0 || rejection.equals("yes")){
                                numRejections++;
                                continue;
                            }
                            //Now loop through all agents
                            for(int a = 0; a < platoonSize[z]; a++){
                                counter++;
                                NodeList agent = nNode.getChildNodes().item(a+2).getChildNodes();
                                double initUtil = Double.valueOf(agent.item(0).getTextContent());
                                double finalUtil = Double.valueOf(agent.item(1).getTextContent());
                                double diffUtil = finalUtil - initUtil;
                                if(diffUtil > maxDif){
                                    maxDif = diffUtil;
                                }
                                if(diffUtil < minDif){
                                    minDif = diffUtil;
                                }
                                diffList.add(diffUtil);
                            }
                        }
                        Collections.sort(diffList);
                        median = diffList.get(diffList.size()/2);
                        double sum = 0;
                        for (Double aDiffList : diffList) {
                            sum += aDiffList;
                        }
                        average = sum / diffList.size();
                        Element minValue = doc.createElement("min");
                        minValue.appendChild(doc.createTextNode(String.valueOf(minDif)));
                        Element maxValue = doc.createElement("max");
                        maxValue.appendChild(doc.createTextNode(String.valueOf(maxDif)));
                        Element avgValue = doc.createElement("average");
                        avgValue.appendChild(doc.createTextNode(String.valueOf(average)));
                        Element mdnValue = doc.createElement("median");
                        mdnValue.appendChild(doc.createTextNode(String.valueOf(median)));
                        sizeElement.appendChild(minValue);
                        sizeElement.appendChild(maxValue);
                        sizeElement.appendChild(avgValue);
                        sizeElement.appendChild(mdnValue);
                    }

                }
                Element rejectionElement = doc.createElement("Rejections");
                rejectionElement.appendChild(doc.createTextNode(String.valueOf(numRejections)));
                Element totalAgentsElement = doc.createElement("TotalAgents");
                totalAgentsElement.appendChild(doc.createTextNode(String.valueOf(counter)));
                rootElement.appendChild(rejectionElement);
                rootElement.appendChild(totalAgentsElement);


                // write the content into xml file
                TransformerFactory transformerFactory = TransformerFactory.newInstance();
                Transformer transformer = transformerFactory.newTransformer();
                transformer.setOutputProperty(OutputKeys.INDENT, "yes");
                DOMSource source = new DOMSource(doc);
                StreamResult result = new StreamResult(f);

                // Output to console for testing
                // StreamResult result = new StreamResult(System.out);

                transformer.transform(source, result);
            }
        } catch (ParserConfigurationException e) {
            e.printStackTrace();
        } catch (SAXException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } catch (TransformerConfigurationException e) {
            e.printStackTrace();
        } catch (TransformerException e) {
            e.printStackTrace();
        }


        //Organize by voting rule
        //Then insert minimum, maximum, average and median values for each platoon size run
        //Append amount of rejections to each file (i.e. Unstable / stable), since this action is not influenced
        //by voting rule
    }
}


