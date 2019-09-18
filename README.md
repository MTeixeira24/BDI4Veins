# BDI4Veins
Integration between the LightJason BDI agent framework and the VEINs vehicular network simulator

## Installation

* Install omnet
    * Download at https://omnetpp.org/download/ 
    * Follow install guide at https://doc.omnetpp.org/omnetpp/InstallGuide.pdf
    * Install dependencies: `sudo apt install build-essential gcc g++ bison flex perl python python3 qt5-default libqt5opengl5-dev tcl-dev tk-dev libxml2-dev zlib1g-dev default-jre doxygen graphviz libwebkitgtk-1.0`
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

## Folder structure

* `/LightJason`, contains the **Java** code for the LightJason, high-level decision making component of the simulator. 
* *  `/LightJason/src/main/java/lightvoting`, contains the J-Voting implementation of the minimax rule
* * `/LightJason/src/main/java/omnet`, contains the project files for the implementation of cooperative vehicle logic and communication with the omnet++ simulator
* * * `/LightJason/src/main/java/omnet/agent` individual agent implementations
* * * `/LightJason/src/main/java/omnet/constants` implementation of constants for use in ASL scripts
* * * `/LightJason/src/main/java/omnet/decision` classes responsible for wrapping and processing agent decisions for exchange with OMNeT++
* * * `/LightJason/src/main/java/omnet/environment` high level models of the environment for use in decision making processes.
* * * `/LightJason/src/main/java/omnet/managers` classes responsible for managing connection with OMNeT++ and delegating requests to agents
* * * `/LightJason/src/main/java/omnet/market` implementation of market based agent logic
* * * `/LightJason/src/main/java/omnet/voting` implementation of agent voting logic
* * * `/LightJason/src/main/java/omnet/statistics` helper classes for the collection of data regarding the agent decision component 
* * `/LightJason/src/main/resources`, static data folder containing configuration files 
* * * `/LightJason/src/main/resources/asl`, agent speak files for defining agent behaviour
* * * `/LightJason/src/main/resources/json`, json formatted configuration files
* `/veinsBDI`, contains the **C++** code for the Omnett++ vehicle communication and kinematic simulations.
* * `/veinsBDI/simulations`, contains all the configuration files to run all implemented simulations
* * `/veinsBDI/src`, source code for all the vehicle communication logic and message exchanging with LightJason
* * * `/veinsBDI/src/app`, application logic, i.e. logic for individual vehicle nodes.
* * * `/veinsBDI/src/managers`, contains classes for communicating with the LightJason agents and parsing/delegating messages to the vehicle nodes
* * * `/veinsBDI/src/maneuvers` includes maneuver logic files, e.g. join maneuver
* * * `/veinsBDI/src/phy` contains implementations and extensions of physical communications models, e.g. nakagami fading.
* * * `/veinsBDI/src/protocols` definition of beaconing protocols
* * * `/veinsBDI/src/scenarios` logic for the simulation scenarios
* * * `/veinsBDI/src/traffic` logic for traffic control and vehicle injection
* * * `/veinsBDI/src/utilities` miscelanous helper classes

## Example application - Bargain agent

Simulation configuration starts by creating a new simulation folder in `veinsBDI/simulations`. For the bargain agent scenario we look in the `laneMergeBargaining` folder. 

### Sumo configuration

Configuration of the road infrastructure is more easily performed in SUMOs `netedit` tool.

All `.xml` files related to sumo are stored in  `sumocfg` folder, with a configuration folder `slm.netc.cfg` in the root simulation folder

### Omnet++ configuration

All of the OMNeT++ configuration files are located within the `veinsBDI/simulations/laneMergeBargaining` folder:

* `config.xml` Specifies the analog and fading models to be used in the wireless communication simulation.
* `slm.rou.xml` Defines vehicle types (e.g. color and engine parameters) and routes.
* `omnetpp.ini` Contains most of the connfiguration and variables of the simulation that will be ran
* `mergeNetwork.ned` Defines the static network components, such as the **traffic manager** and **connection manager**

#### omnetpp.ini

* `*.*.updateInterval = 0.01s` - Definition of the timestep between sending queries to the lightJason agents and the SUMO vehicles
* `*.manager.moduleName = "node"` - The name of the vehicle nodes. Configuration of vehicle node properties are done by refering the defined module name (`node` in this example), e.g. `*.node[*].helper_type = "DynamicPositionHelper"`. See `veinsBDI/simulations/marketPlatoonCruise/omnetpp.ini` for examples on how to configure multiple nodes. 
* `*.node[*].appl_type = "veinsbdi.app.plexeappl.concrete.laneMerge.LaneMergeAgent"` - The application logic to be used by the vehicle nodes, located in `veinsBDI/src/app/plexeappl/concrete/laneMerge`
* `*.node[*].appl.joinManeuver = "JoinAtBackAgent"` - The maneuver logic to be used by the vehicle nodes, implemented in `veinsBDI/src/maneuvers`
* `*.node[*].scenario_type = "veinsbdi.scenarios.LaneMergeScenario"` - The scenario to be followed as defined in `veinsBDI/src/scenarios`
* `*.node[*].appl.asl_file = "bargainAgent.asl"` - The behaviour ASL file that the agent in LightJason is to follow, defined in `/LightJason/src/main/resources/asl`
* `**.traffic_type = "veinsbdi.traffic.LaneTrafficManager"` - The traffic manager that will be used in the simulation, defined in `veinsBDI/src/traffic`


#### mergeNetwork.ned

Network definition file that extends the base `Scenario` network from Plexe. Notable alterations to be made in custom simulations is to update `ljmanager: MergeManager;` from MergeManager to the implemented connection manager that will handle the parsing of messages from LightJason.

### Extending the OMNeT++ Simulation

#### Vehicle Applications

The vehicle application for the bargain agent is defined in the `veinsBDI/src/app/plexeappl/concrete/laneMerge` folder, in the `LaneMergeAgent.cpp` and `LaneMergeAgent.h` files.

All applications that are to communicate with LightJason inherit from the `GeneralPlexeAgentAppl.h` base class and will most likely override the following functions:

* `initialize(int)` - Initialization of several application parameters
* `setInitialBeliefs()` - Called after initilization but before simulation start in order to communicate any initial environment observations to the LightJason agents
* `handleLowerMsg(cMessage*)` - Handling of messages received from lower on the communication stack.
* `handleSelfMsg(cMessage*)` - Handling of self messages, typically timers.

Triggers to be sent to agents are handled using the `Trigger` class, defined in `veinsBDI/src/managers/manager/Trigger/Trigger.h`. Usually the the `Trigger` constructor is called in the format `Trigger {Belief(string), params...}`, where `Belief` is a Macro call that converts the trigger string to the numerical identifier of that string. 

#### Messages

The messages that are used for the bargaining of the lane merge scenario are implemented in `veinsBDI/src/messages/bargain/bargain.msg`. The `.msg` file format is then transpiled by OMNeT++ into source and header files to be included by the vehicle applications.

The bargain message itself inherits from a `MarketMessage` type that is also used in auction scenarios. Any new message type that is to be used in market based strategies should inherit from this base type. However for other types of strategies, such as voting, messages should inherit the `NegotiationMessage` base type.

#### Connection Manager

The connection manager, also known as the LightJason manager, is responsible for sending and receiving messages with LightJason.

All connection managers will inherit from the base `LightJasonManager` class which provides much of the functionality of establishing and maintaining connection with LightJason. The primary functionality to override is:

* `parseResponse(LightJasonBuffer, uint32_t)` - Called by the base class to handle the delegation of messages from the LightJason agents.
*  `setLightJasonParameters()` - Called before simulation start to collect select parameters from the `omnetpp.ini` file to send over to the LightJason server for initial configuration.

The connection manager for the Bargaing Agent in defined in `veinsBDI/src/managers/MergeManager` and overrides the above methods, as well as other base methods for statistics collection.

#### Traffic Manager

The traffic manager is responsible for defining the behaviour of the vehicle injection into the simulation. The traffic manager for the barganing scenario is defined in `veinsBDI/src/traffic` in the `LaneTrafficManager` source and header files.

Traffic managers inherit from the base `TraCIBaseTrafficManager` class.

#### Scenarios

Scenarios are defined in `veinsBDI/src/scenarios`, with the bargain scenario implmented in the `LaneMergeScenario` header and source files. The scenarios define the behaviour an application will have for a certain simulation case. All scenario implementatios will inherit from the `BaseScenario` base class.

### Defining the LightJason agent

To define and implement the agent reasonning the following steps need to performed:

* Create the agent file: `CBargainAgent.java`
* Associate to the agent an ASL file: `bargainAgent.asl`
* Define the manager responsible for managing these types of agents: `CBargainAgentManager.java`
* Update the connection manager to use this Agent manager

#### Agent class file

The bargain agent file is located in `LightJason/src/main/java/jasonveins/omnet/agent/CBargainAgent.java`, and follows the same agent implementation as is explained in the [LightJason Tutorials](https://lightjason.org/tutorials/).

Analogous to the `Trigger` data model used by the vehicle nodes, the agents make use of an `InstructionModel` object to define actions that are to be carried out by their associated vehicle nodes. When an `InstructionModel` is instanciated, it is then added to the queue of instructions to be sent over using the `addInstruction(InstructionModel)` method.

#### Agent behaviour files

The behaviour, i.e. ASL, files for the bargaing agent are located in `LightJason/src/main/resources/asl/bargainAgent.asl`. As with the agent class files, this follows the typical [implementation](https://lightjason.org/tutorials/) of a LightJason. 

The asl file to use with the agent definition is defined in `omnetpp.ini` and is sent to LightJason at simulation start.

#### Agent Managers

An agent manager is responsible for maintaining the agent loop, delegating triggers and queueing instructions.

The agent manager for the bargaining agents is located in `LightJason/src/main/java/jasonveins/omnet/managers/CBargainAgentManager.java`

The bargain agent manager extends the base `AgentManager` and the two main implementations are: 

* `buildAgentBulk()` which defines which agent classes are used and how construction should occur.
* `setSimParams()` parses the simulation parameters that are sent over by OMNeT++, as defined in the `omnetpp.ini`. These are the same parameters, and **follow the same order**, as thoose defined in `setLightJasonParameters()` of the connection manager class in OMNeT++.

When a new agent manager is created, it must be added to the `createManager()` factory method in `LightJason/src/main/java/jasonveins/omnet/managers/ConnectionManager.java`.

## Running simulations

To run simulations first start the LightJason server, which has a main entry point in `LightJason/src/main/java/jasonveins/App.java`.

Next start simulation in OMNeT++ by using the OMNeT++ ide to read the created `omnetpp.ini`. See the [OMNeT++](https://docs.omnetpp.org/), [VEINs](https://veins.car2x.org/tutorial/) and [PLEXE](http://plexe.car2x.org/tutorial/) tutorials first to better understand how OMNeT++ simulations are ran and the usage of the `omnetpp.ini`. 