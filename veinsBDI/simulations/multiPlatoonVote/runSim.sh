#!/bin/bash

RUN=0

while [ $RUN -lt 100  ]
do
	RESULT=2
	until [ $RESULT -eq 0 ]
	do
		../../src/veinsBDI -r $RUN -m -u Cmdenv -c JoinScenario -n ..:../../src:../../../../Plexe/plexe-veins/examples:../../../../Plexe/plexe-veins/src/veins --image-path=../../../../Plexe/plexe-veins/images -l ../../../../Plexe/plexe-veins/src/veins omnetpp.ini
		mv ../../../LightJason/testResults/JoinResults.csv ./ResultsProcessing.csv
		python checkOutliers.py -i ./ResultsProcessing.csv -o ./MergedResults.csv
		if [ $? -eq 0 ]
		then
			break
		fi
		rm ./ResultsProcessing.csv
	done
	rm ./ResultsProcessing.csv
	((RUN++))
done
