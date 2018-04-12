"""Reads in all the .csv files in a directory and calculates:
	-mean time and std dev
	-mean energy and std dev
	-mean avg power and std dev
"""

import csv
import os
import sys
from math import sqrt

NUM_TRIALS = 5


def std_dev(lst, population=True):
    """Calculates the standard deviation for a list of numbers."""
    num_items = len(lst)
    mean = sum(lst) / num_items
    differences = [x - mean for x in lst]
    sq_differences = [d ** 2 for d in differences]
    ssd = sum(sq_differences)
 
    # Note: it would be better to return a value and then print it outside
    # the function, but this is just a quick way to print out the values along
    # the way.
    variance = ssd / (num_items - 1) #Sample std dev (not population)
    sd = sqrt(variance)
    # You could `return sd` here.
 
    print('{} +/- {}.'.format(mean, sd))

def calcstats(times, energies, avgpowers):
	sys.stdout.write("Time: ")
	std_dev(times)
	sys.stdout.write("Energy: ")
	std_dev(energies)
	sys.stdout.write("Avg power: ")
	std_dev(avgpowers)
	print('--------------------------')


items = os.listdir(".")
filenames = []
for name in items:
    if name.endswith(".csv"):
        filenames.append(name.split('.')[0]) #don't include the file extension
#print filenames

#Read in all the data from the .csv files.
for filename in filenames:
	#Multipliers for slower ARM CPU frequencies
	multiplier = 1
	if("333" in filename):
		multiplier = 2
	if("166" in filename):
		multiplier = 4
	times = []
	energies = []
	avgpowers = []
	with open('{}.csv'.format(filename), 'rb') as csvfile:
		csvreader = csv.reader(csvfile, delimiter=',', quotechar='|')
		for line in csvreader:
			time = float(line[0]) * multiplier
			energy = float(line[1])
			times.append(time)
			energies.append(energy)
			avgpowers.append(energy / time) #avg power = delta energy / delta time
	print("Configuration: " + filename)
	calcstats(times, energies, avgpowers)




