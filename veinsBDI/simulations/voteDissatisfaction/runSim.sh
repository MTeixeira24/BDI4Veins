#!/bin/bash
RUN=0
while [ $RUN -lt 150  ]
do
	../../src/veinsBDI -r $RUN -m -u Cmdenv -c MiniSumScenario -n ..:../../src:../../../../Plexe/plexe-veins/examples:../../../../Plexe/plexe-veins/src/veins --image-path=../../../../Plexe/plexe-veins/images -l ../../../../Plexe/plexe-veins/src/veins omnetpp.ini
    ((RUN++))
done
RUN=0
while [ $RUN -lt 50  ]
do
    ../../src/veinsBDI -r $RUN -m -u Cmdenv -c MiniMaxScenario -n ..:../../src:../../../../Plexe/plexe-veins/examples:../../../../Plexe/plexe-veins/src/veins --image-path=../../../../Plexe/plexe-veins/images -l ../../../../Plexe/plexe-veins/src/veins omnetpp.ini
	((RUN++))
done