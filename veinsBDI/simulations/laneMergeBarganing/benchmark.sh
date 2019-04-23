#!/bin/bash

if [ ! -f csvResults/TimeValues.csv ]; then
    touch csvResults/TimeValues.csv
    echo VehicleCount,Run,Time >> csvResults/TimeValues.csv
fi

if [ ! -f csvResults/TimeStepOmnetpp.csv ]; then
    touch csvResults/TimeStepOmnetpp.csv
    echo VehicleCount,Run,Time >> csvResults/TimeStepOmnetpp.csv
fi

if [ ! -f csvResults/SumoDelays.csv ]; then
    touch csvResults/SumoDelays.csv
    echo VehicleCount,Run,Time,Type >> csvResults/SumoDelays.csv
fi

T1=$(cat omnetpp.ini | grep vPairs | head -1  | awk '{print $5}')
T2=$(cat omnetpp.ini | grep vLanes | head -1  | awk '{print $5}')
vCount=$(( $T1 * $T2 *2  ))

vCount=50

while [ $vCount -lt 1001 ]
do
    RUN=0
    while [ $RUN -lt 100 ]
    do
        /usr/bin/time -p -o tempfile.txt  opp_run -r $RUN -m -u Cmdenv -c vehicles$vCount\
        -n ..:../../src:../../../../PlexeSrc/plexe-veins/examples/veins:../../../../PlexeSrc/plexe-veins/src/veins\
        --image-path=../../../../PlexeSrc/plexe-veins/images -l ../../src/veinsBDI\
        -l ../../../../PlexeSrc/plexe-veins/src/veins --cmdenv-redirect-output=true  omnetpp.ini 2>error\
        && cat tempfile.txt | grep real | echo $vCount,$RUN,$(cut -d ' ' -f 2) >> csvResults/TimeValues.csv

        RESULTFILE="results/*"
        RESULTFILE+=$RUN
        RESULTFILE+=".out"

        cat $RESULTFILE\
            | grep simsec/sec | cut -d $'\n' -f 2 | awk '{print $3}'\
            | echo $vCount,$RUN,$(cut -d '=' -f 2) >> csvResults/TimeStepOmnetpp.csv

        #python parseXML.py $RUN $vCount

        #rm SumoSummary.xml

        SUMOBUFDELAY=$(cat sumoTimeStep.temp | grep BUF | awk '{print $2}')
        SUMOTIMESTEPDELAY=$(cat sumoTimeStep.temp | grep TIM | awk '{print $2}')

        echo $vCount,$RUN,$SUMOBUFDELAY,Buffering >> csvResults/SumoDelays.csv 
        echo $vCount,$RUN,$SUMOTIMESTEPDELAY,TimeStep >> csvResults/SumoDelays.csv 

        ((RUN++))
    done
    tar -zcvf csvResults/results$vCount.tar.gz results/
    cp csvResults/results$vCount.tar.gz csvResults/backup
    mv csvResults/results$vCount.tar.gz csvResults/tars
    vCount=$(( $vCount + 100 ))
    rm -rf results/
done


rm sumoTimeStep.temp
rm tempfile.txt
