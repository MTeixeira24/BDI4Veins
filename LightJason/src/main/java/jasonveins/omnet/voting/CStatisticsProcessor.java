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
    static String[] fileNames = {"Unstable", "Stable"};
    static String[] voteRules = {"Borda", "Approval", "Copeland", "Plurality"};
    static int[] platoonSize = {4,5,6,7,8};
    static ArrayList<Abnormality> abnormalities = new ArrayList<>();

    public static void main(final String[] p_args){
        //DataProcessor dp = new DataProcessor();

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
                        ArrayList<Double> exitPercentages = new ArrayList<>();
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
                            int platoonExits = 0;
                            Node nNode = iterations.item(iter);
                            Node pNode = nNode.getChildNodes().item(0);
                            String stringSpeed = pNode.getAttributes().getNamedItem("finalSpeed").getTextContent();
                            int finalPlatoonSpeed = Integer.valueOf(stringSpeed);
                            String rejection = nNode.getChildNodes().item(1).getTextContent();
                            if(rejection.equals("yes")){
                                numRejections++;
                                continue;
                            }
                            //See if this is an abnormal ocurence
                            if(finalPlatoonSpeed == 0){
                                Abnormality ab = new Abnormality(iter, platoonSize[z], voteRules[j], fileNames[i]);
                                abnormalities.add(ab);
                                continue;
                            }
                            //Now loop through all agents
                            for(int a = 0; a < platoonSize[z]; a++){
                                counter++;
                                Node agent = nNode.getChildNodes().item(a+2);
                                if(agent == null){
                                    Abnormality ab = new Abnormality(iter, platoonSize[z], voteRules[j], fileNames[i]);
                                    abnormalities.add(ab);
                                    break;
                                }
                                double initUtil = Double.valueOf(agent.getChildNodes().item(0).getTextContent());
                                double finalUtil = Double.valueOf(agent.getChildNodes().item(1).getTextContent());
                                double diffUtil = finalUtil - initUtil;
                                if(diffUtil > maxDif){
                                    maxDif = diffUtil;
                                }
                                if(diffUtil < minDif){
                                    minDif = diffUtil;
                                }
                                diffList.add(diffUtil);
                                if(finalUtil < 0.8) platoonExits++;
                            }
                            exitPercentages.add((double)platoonExits/platoonSize[z]);
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


                        Element avgRejectionElement = doc.createElement("AverageRejection");
                        sum = 0;
                        for(Double r : exitPercentages) sum += r;
                        avgRejectionElement.appendChild(doc.createTextNode(String.valueOf(100*(sum/exitPercentages.size()))));
                        sizeElement.appendChild(avgRejectionElement);
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

        for(Abnormality ab : abnormalities){
            System.out.println(ab.toString());
        }
        System.out.println("There is a total of " + abnormalities.size() + " abnormalities.");
        //Organize by voting rule
        //Then insert minimum, maximum, average and median values for each platoon size run
        //Append amount of rejections to each file (i.e. Unstable / stable), since this action is not influenced
        //by voting rule
        mergeAll();
    }

    public static void mergeAll() {
        try {
            DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
            DocumentBuilder docBuilder = docFactory.newDocumentBuilder();
            Document doc = docBuilder.newDocument();
            //File to store information
            File f = new File("mergedResults.xml");
            //Root of file
            Element rootElement = doc.createElement("Results");
            doc.appendChild(rootElement);

            for (int i = 0; i < fileNames.length; i++) {
                for (int j = 0; j < voteRules.length; j++) {
                    for (int z = 0; z < platoonSize.length; z++) {
                        //Get the file
                        File p = new File("testResults/" + fileNames[i] + "/" + fileNames[i] + voteRules[j] + platoonSize[z] + ".xml");
                        Document parseDoc = docBuilder.parse(p);
                        parseDoc.getDocumentElement().normalize();

                        Element scenario = doc.createElement("Scenario");
                        Element type = doc.createElement("Type");
                        type.setTextContent(fileNames[i]);
                        scenario.appendChild(type);

                        Element rule = doc.createElement("Rule");
                        rule.setTextContent(voteRules[j]);
                        scenario.appendChild(rule);

                        Element psize = doc.createElement("platoonSize");
                        psize.setTextContent(String.valueOf(platoonSize[z]));
                        scenario.appendChild(psize);

                        NodeList iterations = parseDoc.getDocumentElement().getElementsByTagName("Iteration");
                        for(int a = 0; a < iterations.getLength(); a++){
                            Element iter = (Element)iterations.item(a);
                            String iterNumber = iter.getAttributes().getNamedItem("number").getNodeValue();

                            Element iteration = doc.createElement("Iteration");
                            Element number = doc.createElement("Number");
                            number.setTextContent(iterNumber);
                            iteration.appendChild(number);



                            String intialSpeed = iter.getElementsByTagName("Platoon").item(0).getAttributes().getNamedItem("initialSpeed").getNodeValue();
                            String finalSpeed = iter.getElementsByTagName("Platoon").item(0).getAttributes().getNamedItem("finalSpeed").getNodeValue();

                            Element is = doc.createElement("InitialSpeed");
                            Element fs = doc.createElement("FinalSpeed");
                            is.setTextContent(intialSpeed);
                            fs.setTextContent(finalSpeed);
                            iteration.appendChild(is);
                            iteration.appendChild(fs);

                            Node clone = iter.cloneNode(true);
                            doc.adoptNode(clone);
                            //scenario.appendChild(clone);
                            Element eClone = (Element)clone;
                            iteration.appendChild(eClone.getElementsByTagName("Rejected").item(0));
                            NodeList agentList = eClone.getElementsByTagName("Agent");

                            for(int b = 0; b < platoonSize[z]; b++){

                                Element curAgent = (Element)agentList.item(b);

                                Element agent = doc.createElement("Agent");
                                String aId = curAgent.getAttributes().getNamedItem("id").getNodeValue();

                                Element idElement = doc.createElement("Id");
                                idElement.setTextContent(aId);
                                agent.appendChild(idElement);


                                agent.appendChild(curAgent.getElementsByTagName("initialUtility").item(0));
                                agent.appendChild(curAgent.getElementsByTagName("finalUtility").item(0));
                                //Node test1 = cloneList.item(2+b).getChildNodes().item(0);
                                //agent.appendChild(cloneList.item(2+b).getChildNodes().item(0));

                                iteration.appendChild(agent);

                            }
                            scenario.appendChild(iteration);
                        }

                        rootElement.appendChild(scenario);
                    }
                }
            }

            // write the content into xml file
            TransformerFactory transformerFactory = TransformerFactory.newInstance();
            Transformer transformer = transformerFactory.newTransformer();
            DOMSource source = new DOMSource(doc);
            StreamResult result = new StreamResult(f);

            // Output to console for testing
            // StreamResult result = new StreamResult(System.out);


            transformer.transform(source, result);
        } catch(TransformerException e){
            e.printStackTrace();
        } catch(ParserConfigurationException e){
            e.printStackTrace();
        } catch(SAXException e){
            e.printStackTrace();
        } catch(IOException e){
            e.printStackTrace();
        }
    }

        static class Abnormality{
        int iteration;
        int size;
        String rule;
        String type;

        Abnormality(int iteration, int size, String rule, String type){
            this.iteration = iteration;
            this.size = size;
            this.rule = rule;
            this.type = type;
        }

        public String toString(){
            return "Abnormality detected in iteration " + this.iteration + " of size " + this.size + " in rule " + this.rule
                    + " of type " + this.type;
        }
    }
}


