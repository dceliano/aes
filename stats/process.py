#!/usr/bin/env python
# Processes energy and time log files and computes stats from them.
# Only works on one CPU/FPGA frequency pair at a time.

from collections import defaultdict
from dateutil.parser import parse
import datetime
import math

IDLE_1V_ENERGY = 32 #Approximately the amount of energy we measure on the Parallela's 1v rail when it is idle
CPU_FREQ = 666
FPGA_FREQ = 250
#TIME_SCALE = 1

# Function to compute mean and standard deviation given a list of numbers
def stat(xs):
  s = sum(xs)
  n = float(len(xs))
  m = s / n #mean
  var = 0
  for x in xs:
    var += (x - m) * (x - m)
  return m, math.sqrt(var / (n - 1))


# Main program

#Read in the energy measurements for this frequency pair
with open('energy_%d_%d.log' % (CPU_FREQ, FPGA_FREQ)) as f:
  lines = f.readlines()
  startTime = parse(lines[0])
  energy = [[w.strip() for w in l.strip().split(', ')] for l in lines[1:]] #array of [5V energy, 1V energy]


#Read in the time measurements for this frequency pair
benches = [] #array to hold measured timing info
with open('timing_%d_%d.log' % (CPU_FREQ, FPGA_FREQ)) as t:
  lines = t.readlines()
  t0 = parse(lines[0].split(' ')[0]).replace(tzinfo=None) #t0 is the time when the first encryption started
  for i, line in zip(range(len(lines)), lines): #For all the timestamps
    start, end, dt = line.split(' ')
    #not sure what TIME_SCALE is supposed to do.
    start = t0 + (parse(start).replace(tzinfo=None) - t0) * TIME_SCALE
    end = t0 + (parse(end).replace(tzinfo=None) - t0) * TIME_SCALE
    dt = float(dt.strip()) * TIME_SCALE
    benches.append((start, end, dt, i))

#Get rid of energy measurements which didn't take place while we were encrypting.
data = defaultdict(list)
for t0, _, v1 in energy:
  t = startTime + datetime.timedelta(seconds=float(t0))
  for (start, end, dt, idx) in benches:
    #Only register an energy measurement if it takes place while we're encrypting
    if start <= t and t <= end:
      data[idx].append((max(float(v1) - IDLE_1V_ENERGY, 0), dt))

#No clue what this does
grouped = defaultdict(list)
for idx, xs in data.items():
  grouped[int(idx / 5)].append((
      sum(v1 for v1, _ in xs),
      xs[0][1]
  ))

#Compute stats and print them out
for idx, xs in grouped.items():
  print(stat([v1 * 64/77 for v1, _ in xs]), stat([t * 64/77 for _, t in xs]))


