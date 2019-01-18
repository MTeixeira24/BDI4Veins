from pandas import DataFrame, read_csv
import pandas as pd 
import sys 
import numpy
import scipy.stats as sp
import math
from pylab import *
import sys, getopt

def main(argv):
    try:
      opts, args = getopt.getopt(argv,"hi:o:",["input=","output="])
    except getopt.GetoptError:
        print('usage: checkOutliers.py -i <Results from simulation run> -o <Merged CSV file>')
        sys.exit(2)
    if len(sys.argv) < 2:
        print('usage: checkOutliers.py -i <Results from simulation run> -o <Merged CSV file>')
        sys.exit(2)
    for opt, arg in opts:
        if opt in ("-i", "--input"):
            inFile = str(arg)
        elif opt in ("-o", "--output"):
            outFile = str(arg)
    rdf = pd.read_csv(inFile)
    platoonSize = int(rdf[rdf.Agent == 0].PlatoonSize)
    #First, check if the second utility measures are zero
    secondMeasure = 100
    for a in range(0, platoonSize):
        secondMeasure *= float(rdf[rdf.Agent == a].Utility2)
    print (secondMeasure)
    if secondMeasure == 0.0:
        print("MALFORMED, second measure of utility non existant!")
        sys.exit(1)
    #Next, are all measures of the joiners zero?
    joiners = [platoonSize, platoonSize + 1, platoonSize + 2]
    secondMeasure = 0.0
    for j in joiners:
        secondMeasure += float(rdf[rdf.Agent == j].Utility2)
    if secondMeasure == 0.0:
        print("MALFORMED, no measure of utility found in joiners!")
        sys.exit(1)
    try:
        mdf = pd.read_csv(outFile)
        mdf = mdf.append( rdf, ignore_index = True )
        mdf.to_csv(outFile,index=False)
        print("File exists")
    except FileNotFoundError:
        print("File not found")
        rdf.to_csv(outFile,index=False)
    print("CSV is well formed")
    sys.exit(0)

if __name__ == "__main__":
    main(sys.argv[1:])