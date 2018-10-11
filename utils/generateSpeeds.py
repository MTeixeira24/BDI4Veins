#!/usr/bin/python

#Creates a json file with a random number of speeds
#{
#   "stable": //Type of distribution
#       {
#           "4": //Size of the platoon
#            {
#               0: //Iteration number
#               {
#                   [100,110,95,90] //Speed array
#               }
#            }         
#       }
#
#   "unstable":{}

import json
import random
import sys, getopt


def main(argv):
    data = {}
    stable = {}
    unstable = {}
    minspeed = 0
    maxspeed = 0
    minplatoonsize = 0
    maxplatoonsize = 0
    joiners = 0
    try:
      opts, args = getopt.getopt(argv,"hn:m:l:p:j:",["minspeed=","maxspeed=","minplatoonsize=", "maxplatoonsize=","joiners="])
    except getopt.GetoptError:
        print('usage: generateSpeeds.py -n <minSpeed> -m <maxSpeed> -l <minPlatoonSize> -p <maxPlatoonSize> -j <numberOfJoiners>')
        sys.exit(2)
    if len(sys.argv) < 5:
        print('usage: generateSpeeds.py -n <minSpeed> -m <maxSpeed> -l <minPlatoonSize> -p <maxPlatoonSize> -j <numberOfJoiners>')
        sys.exit(2)
    for opt, arg in opts:
        if opt in ("-n", "--minspeed"):
            minspeed = int(arg)
        elif opt in ("-m", "--maxspeed"):
            maxspeed = int(arg)
        elif opt in ("-l", "--minplatoonsize"):
            minplatoonsize = int(arg)
        elif opt in ("-p", "--maxplatoonsize"):
            maxplatoonsize = int(arg)
        elif opt in ("-j", "--joiner"):
            joiners = int(arg)
    
    if(minspeed > maxspeed or minplatoonsize < 4 or minplatoonsize > maxplatoonsize or joiners < 1):
        print('Invalid inputs')
        sys.exit(2)

    for platoonSize in range(minplatoonsize,maxplatoonsize + 1):
        iterations = {}
        for iteration in range(0,21):
            speeds = []
            if platoonSize == 4:
                speeds = [90,90,100,100]
            elif platoonSize == 5:
                speeds = [90,90,95,100,100]
            elif platoonSize == 6:
                speeds = [90,90,95,95,100,100]
            elif platoonSize == 7:
                speeds = [90,90,90,95,100,100,100]
            elif platoonSize == 8:
                speeds = [90,90,90,95,95,100,100,100]
            elif platoonSize == 9:
                speeds = [90,90,90,95,95,95,100,100,100]
            elif platoonSize == 10:
                speeds = [90,90,90,90,95,95,100,100,100,100]
            elif platoonSize == 11:
                speeds = [90,90,90,90,95,95,95,100,100,100,100]
            elif platoonSize == 12:
                speeds = [90,90,90,90,95,95,95, 95,100,100,100,100]
            else:
                for speed in range(0,platoonSize):
                    speeds.append(random.randint(minspeed,maxspeed))
            #joiner speed
            for j in range(0, joiners):
                speeds.append(random.randint(minspeed,maxspeed))
            iterations.update({iteration: speeds})
        stable.update({platoonSize: iterations})

    data.update({'Stable': stable})

    for platoonSize in range(minplatoonsize,maxplatoonsize + 1):
        iterations = {}
        for iteration in range(0,21):
            speeds = []
            for speed in range(0,platoonSize):
                speeds.append(random.randint(minspeed,maxspeed))
            #joiner speed
            for j in range(0, joiners):
                speeds.append(random.randint(minspeed,maxspeed))
            iterations.update({iteration: speeds})
        unstable.update({platoonSize: iterations})

    data.update({'Unstable': unstable})


    with open('preferedSpeeds.json', 'w') as outfile:
        json.dump(data, outfile, indent=4)

if __name__ == "__main__":
    main(sys.argv[1:])
