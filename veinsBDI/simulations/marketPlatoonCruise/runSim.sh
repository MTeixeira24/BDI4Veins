#!/bin/bash
RUN=0
BUF=1
while [ $RUN -lt 400  ]
do
    until [ $BUF -eq 0 ]
	do
        ../../src/veinsBDI -r $RUN -m -u Cmdenv -c BaseCruiseScenario -n ..:../../src:../../../../Plexe/plexe-veins/examples:../../../../Plexe/plexe-veins/src/veins --image-path=../../../../Plexe/plexe-veins/images -l ../../../../Plexe/plexe-veins/src/veins omnetpp.ini
		if [ $? -eq 0 ]
		then
            echo "Succesful run"
			break
		fi
    echo "Bad Run"
    sleep 1
	done
	((RUN++))
    sleep 1
done
mv ../../../LightJason/testResults/MarketResults.csv ./MarketCruise.csv




	