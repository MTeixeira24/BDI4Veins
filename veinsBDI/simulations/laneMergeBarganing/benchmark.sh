#!/bin/bash

if [ ! -f csvResults/TimeValues.csv ]; then
    touch TimeValues.csv
    echo VehicleCount,Run,Time >> TimeValues.csv
fi

if [ ! -f csvResults/TimeStepOmnetpp.csv ]; then
    touch TimeStepOmnetpp.csv
    echo VehicleCount,Run,Time >> TimeStepOmnetpp.csv
fi

if [ ! -f csvResults/SumoDelays.csv ]; then
    touch SumoDelays.csv
    echo VehicleCount,Run,Time,Type >> TimeStepOmnetpp.csv
fi

T1=$(cat omnetpp.ini | grep vPairs | head -1  | awk '{print $5}')
T2=$(cat omnetpp.ini | grep vLanes | head -1  | awk '{print $5}')
vCount=$(( $T1 * $T2 *2  ))

vCount=50

while [ $cCount -lt 551 ]
do
    while [ $RUN -lt 100 ]
    do
        /usr/bin/time -p -o tempfile.txt  opp_run -r $RUN -m -u Cmdenv -c vehicles$vCount\
        -n ..:../../src:../../../../PlexeSrc/plexe-veins/examples/veins:../../../../PlexeSrc/plexe-veins/src/veins\
        --image-path=../../../../PlexeSrc/plexe-veins/images -l ../../src/veinsBDI\
        -l ../../../../PlexeSrc/plexe-veins/src/veins --cmdenv-redirect-output=true  omnetpp.ini\
        && cat tempfile.txt | grep real | echo $vCount,$RUN,$(cut -d ' ' -f 2) >> csvResults/TimeValues.csv

        RESULTFILE="BaseScenario-nCars=15,platoonSize=4,nLanes=1,vtype=vBargain,vPairs=25,vLanes=4,25_#2a_4_#2a_2-#"
        RESULTFILE+=$RUN
        RESULTFILE+=".out"

        cat results/$RESULTFILE\
            | grep simsec/sec | cut -d $'\n' -f 2 | awk '{print $3}'\
            | echo $vCount,$RUN,$(cut -d '=' -f 2) >> csvResults/TimeStepOmnetpp.csv

        python parseXML.py $RUN $vCount

        rm SumoSummary.xml

        SUMOBUFDELAY=$(cat sumoTimeStep.temp | grep BUF | awk '{print $2}')
        SUMOTIMESTEPDELAY=$(cat sumoTimeStep.temp | grep TIM | awk '{print $2}')

        echo $vCount,$RUN,$SUMOBUFDELAY,Buffering >> csvResults/SumoDelays.csv 
        echo $vCount,$RUN,$SUMOTIMESTEPDELAY,TimeStep >> csvResults/SumoDelays.csv 

        ((RUN++))
    done
    tar -zcvf csvResults/results$vCount.tar.gz results/
    vCount=$(( $vCount + 50 ))
    rm -rf results/
done


rm sumoTimeStep.temp
rm tempfile.txt
