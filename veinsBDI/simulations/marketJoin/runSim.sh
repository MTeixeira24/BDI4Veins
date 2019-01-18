#!/bin/bash

RUN=0
RESULT=0
while [ $RUN -lt 500  ]
do
	until [ $RESULT -eq 1 ]
	do
		../../src/veinsBDI -r $RUN -m -u Cmdenv -c BaseJoinScenario -n ..:../../src:../../../../Plexe/plexe-veins/examples:../../../../Plexe/plexe-veins/src/veins --image-path=../../../../Plexe/plexe-veins/images -l ../../../../Plexe/plexe-veins/src/veins omnetpp.ini
		mv ../../../LightJason/testResults/MarketResults.csv ./ResultsProcessing.csv
		python checkOutliers.py -i ./ResultsProcessing.csv -o ./MergedResults.csv
		if [ $? -eq 0 ]
		then
		    rm ./ResultsProcessing.csv
			break
		fi
	    rm ./ResultsProcessing.csv
        sleep 2
	done
	((RUN++))
done
