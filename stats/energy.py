#!/usr/bin/env python
# Takes energy measurements using bognor over a TCP connection on port 2002 (application layer protocol is Telnet).
# Whenever "cmd" is sent to bognor, bognor sends back its 5v and 1v power measurements in the same line.
# In this script, we poll the power (roughly) every 100ms.

import socket
import time
import sys
import datetime


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('bognor.sm', 2002))

print datetime.datetime.now() #Used to correlate power measurements with AES execution time

t0 = time.time() #initial time measurement
v1p = None #1V power rail measurement
v5p = None #5V power rail measurement
while True:
  s.send('cmd\n')
  #v1c and v5c are the power measurements just made.
  #v1p and v5p are the cumulative power measurements
  #v1 and v5 record the amount of power used in the last cycle, and are the values which are printed
  v5c, v1c = map(int, s.recv(1024).strip().split('=')[1].split('!')[:2])
  if v1p is None or v5p is None:
    v1p = v1c
    v5p = v5c
  else:
    v1 = v1c - v1p
    v5 = v5c - v5p
    v1p, v5p = v1c, v5c
    if v5 != 0 and v1 != 0:
      print time.time() - t0, ',', v5, ',',  v1
      sys.stdout.flush()

  time.sleep(0.1)

s.close()
