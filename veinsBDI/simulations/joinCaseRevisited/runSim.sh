#!/bin/bash
RUN=0

while [ $RUN -lt 1680  ]
do
    opp_run -r 0 -m -u Cmdenv -c MiniMaxScenario -n ..:../../src:../../../../PlexeSrc/plexe-veins/examples/veins:../../../../PlexeSrc/plexe-veins/src/veins --image-path=../../../../PlexeSrc/plexe-veins/images -l ../../src/veinsBDI -l ../../../../PlexeSrc/plexe-veins/src/veins omnetpp.ini
	((RUN++))
done

