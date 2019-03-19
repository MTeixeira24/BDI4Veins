import xml.etree.ElementTree
import numpy as np
import sys
import os

fileName = "sumoStats.csv"
e = xml.etree.ElementTree.parse('SumoSummary.xml').getroot()
isFirst = True
a1 = 0
a2 = 0
processingTimes = []

for step in e.findall('step'):
    if isFirst:
        isFirst = False
        a1 = step.get("duration")
    else:
        a2 = step.get("duration")
        processingTimes.append(int(a2) - int(a1))
        a1 = a2

npArray = np.array(processingTimes)

with open(fileName, 'a') as file:
    try:
        if os.stat(fileName).st_size == 0:
            file.write("VehicleCount,Run,Mean,Median,Max,Min\n")
    except FileNotFoundError:
        file.write("VehicleCount,Run,Mean,Median,Max,Min\n")
    runNumber = sys.argv[1]
    vehicleCount = sys.argv[2]
    mn = np.min(npArray)
    mx = np.max(npArray)
    mean = np.average(npArray)
    median = np.median(npArray)
    file.write(vehicleCount+","+runNumber+","+str(mean)+","+str(median)+","+str(mx)+","+str(mn)+"\n")
