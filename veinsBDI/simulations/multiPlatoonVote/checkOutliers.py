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
    outliers = len(rdf[rdf.Utility2 == 0])
    print("Outliers: " + str(outliers))
    if outliers == 0:
        try:
            mdf = pd.read_csv(outFile)
            mdf = mdf.append( rdf, ignore_index = True )
            mdf.to_csv(outFile,index=False)
            print("File exists")
        except FileNotFoundError:
            print("File not found")
            rdf.to_csv(outFile,index=False)
    else:
        print("MALFORMED")
        sys.exit(1)
    print("CSV is well formed")
    sys.exit(0)

if __name__ == "__main__":
    main(sys.argv[1:])