#!/bin/sh
# Runs unaccelerated trials of AES encryption on the Parallela card for each of several different modes.
# (different CPU frequencies)

KEY=00112233445566778899aabbccddeeff
IV=00112233445566778899aabbccddeeff
INPUT=plaintext
OUTPUT=output.txt
ITERS=5


for ((i = 0; i < $ITERS; ++i)) do
  # devmem2 0xF8000120 w 0x1F000200 #change the CPU frequency
  ./aes $KEY $IV $INPUT $OUTPUT
done
