#!/bin/bash

RUN=0

if [ ! -f TimeValues.csv ]; then
    touch TimeValues.csv
    echo VehicleCount,Run,Time >> TimeValues.csv
fi

if [ ! -f TimeStepOmnetpp.csv ]; then
    touch TimeStepOmnetpp.csv
    echo VehicleCount,Run,Time >> TimeStepOmnetpp.csv
fi

T1=$(cat omnetpp.ini | grep vPairs | awk '{print $3}')
T2=$(cat omnetpp.ini | grep lanePairs | awk '{print $3}')
vCount=$(( $T1 * $T2 *2  ))


while [ $RUN -lt 1 ]
do
    /usr/bin/time -p -o tempfile.txt  opp_run -r $RUN -m -u Cmdenv -c BaseScenario\
    -n ..:../../src:../../../../PlexeSrc/plexe-veins/examples/veins:../../../../PlexeSrc/plexe-veins/src/veins\
    --image-path=../../../../PlexeSrc/plexe-veins/images -l ../../src/veinsBDI\
    -l ../../../../PlexeSrc/plexe-veins/src/veins --cmdenv-redirect-output=true  omnetpp.ini\
    && cat tempfile.txt | grep real | echo $vCount,$RUN,$(cut -d ' ' -f 2) >> TimeValues.csv

    RESULTFILE="BaseScenario-nCars=15,platoonSize=4,nLanes=1,vtype=vBargain-#"
    RESULTFILE+=$RUN
    RESULTFILE+=".out"

    cat results/$RESULTFILE\
	    | grep simsec/sec | cut -d $'\n' -f 2 | awk '{print $3}'\
	    | echo $vCount,$RUN,$(cut -d '=' -f 2) >> TimeStepOmnetpp.csv

    python parseXML.py $RUN $vCount

    rm SumoSummary.xml

    ((RUN++))
done

rm tempfile.txt
