#!/bin/sh
# Runs 5 unaccelerated trials of AES encryption on the Parallela card for each of several different modes.
# (different CPU frequencies)

KEY=00112233445566778899aabbccddeeff
IV=00112233445566778899aabbccddeeff
FILE=input16.txt
ITERS=5


for ((i = 0; i < $ITERS; ++i)) do
  ./aes/unix/aes $KEY $IV $FILE
done
