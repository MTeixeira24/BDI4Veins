# BDI4Veins
Integration between the LightJason BDI agent framework and the VEINs vehicular network simulator

## Installation

* Install omnet
    * Download at https://omnetpp.org/download/ 
    * Follow install guide at https://doc.omnetpp.org/omnetpp/InstallGuide.pdf
    * `. setenv`
    * `/configure WITH_OSGEARTH=no WITH_OSG=no`
    * `make -j <cpu cores>`
    * Make sure omnet is added to path in the .bashrc file
    * `export PATH=<omnet install path>/bin:$PATH`
    * As of writing, Omnet will only launch with java 8
 
* Install Plexe-Veins
    * Install dependencies: `sudo apt install libxerces-c-dev libgdal-dev libproj-dev libfox-1.6-dev`
    * Follow http://plexe.car2x.org/tutorial/
* Clone the project

## IDE setup - OMNeT++
* Start omnetpp
* Import the veinsBDI project located inside the BDI4Veins root folder
* Import the plexe-veins project
* In the project explorer, right click the root veinsBDI folder and select properties. 
* Go to OMNeT++, Makemake, select the veinsBDI folder. Under build select Makemake and  press Options
* Under compile, add the plexe-veins directory.
* Back in the Makemake dialog window, select the src folder and tick the makemake option
* Click the options button and select shared library
* Apply and exit
* Build the workspace by using Ctrl+B

## IDE setup - Intellij
Note: the choice for IntelliJ is arbitrary. Any java IDE with maven support can be used for development
* Install and ensure that IntelliJ has support for git and maven
* Open the import project helper
* Select the LightJason folder inside the BDI4Veins folder
* Select import project from external sources and select Maven
* Tick Import Maven projects automatically and set to automatically download sources and documentation, click next
* Two projects to import will appear. Select only the topmost project.
* Finish importing

## Visual studio code setup for ASL development
TODO

## Example application - Bargain agent
Development of a bargain agent. TODO

## Sumo configuration
TODO
## Omnet++ configuration
TODO

#Defining the agent
* Create the agent file: CBargainAgent.java
* Associate to the agent an ASL file: bargainAgent.asl
* Define the manager responsible for managing these types of agents: CBargainAgentManager.java
* Update the connection manager to use this Agent manager
