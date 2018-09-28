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


data = {}
stable = {}
unstable = {}
for platoonSize in range(4,9):
    iterations = {}
    for iteration in range(0,21):
        speeds = []
        for speed in range(0,platoonSize):
            speeds.append(random.randint(90,110))
        iterations.update({iteration: speeds})
    stable.update({platoonSize: iterations})

data.update({'Stable': stable})

for platoonSize in range(4,9):
    iterations = {}
    for iteration in range(0,21):
        speeds = []
        for speed in range(0,platoonSize):
            speeds.append(random.randint(85,115))
        iterations.update({iteration: speeds})
    unstable.update({platoonSize: iterations})

data.update({'Unstable': unstable})


with open('preferedSpeeds.json', 'w') as outfile:
    json.dump(data, outfile, indent=4)
