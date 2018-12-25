import json
import random
import sys, getopt
import numpy as np
import math

def main(argv):
    data = {}
    eMean = 200
    eVar = 25
    wMean = 50
    wVar = 20
    minplatoonsize = 4
    maxplatoonsize = 14
    joiners = 5
    nIterations = 100


    for platoonSize in range(minplatoonsize,maxplatoonsize + 1):
        iterations = {}
        for iteration in range(0,nIterations + 1):
            parameters = []
            for agent in range(0,platoonSize + joiners):
                parameters.append(
                    {
                        "endowment": math.floor( np.random.normal(eMean, eVar) ),
                        "wtp": math.floor( np.random.normal(wMean, wVar) )
                    }
                )
            iterations.update({iteration: parameters})
        data.update({platoonSize: iterations})
    

    with open('marketParameters.json', 'w') as outfile:
        json.dump(data, outfile, indent=4)

if __name__ == "__main__":
    main(sys.argv[1:])
