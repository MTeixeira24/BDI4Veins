#!/bin/bash

RUN=0

while [ $RUN -lt 30 ]
do
    /usr/bin/time -p -o tempfile.txt  opp_run -r 0 -m -u Cmdenv -c BaseScenario\
    -n ..:../../src:../../../../PlexeSrc/plexe-veins/examples/veins:../../../../PlexeSrc/plexe-veins/src/veins\
    --image-path=../../../../PlexeSrc/plexe-veins/images -l ../../src/veinsBDI\
    -l ../../../../PlexeSrc/plexe-veins/src/veins omnetpp.ini\
    && cat tempfile.txt | grep real | echo $RUN,$(cut -d ' ' -f 2),$1 >> timeresults.csv

    ((RUN++))
done

rm tempfile.txt
