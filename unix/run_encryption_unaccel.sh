#!/bin/bash
# Runs unaccelerated trials of AES encryption on the Parallela card for each of several different modes.
# (different CPU frequencies)

KEY=00112233445566778899aabbccddeeff
IV=00112233445566778899aabbccddeeff
INPUT=plaintext
OUTPUT=output.txt
ITERS=5

> output_666_0.csv #make an empty file
> output_333_0.csv #make an empty file
> output_166_0.csv #make an empty file
for ((i = 0; i < $ITERS; ++i)) do
  devmem2 0xF8000120 w 0x1F000200 #change the CPU frequency to 666MHz
  ./aes $KEY $IV $INPUT $OUTPUT >> output_666_0.csv #append measurements to the file
  devmem2 0xF8000120 w 0x1F000400 #change the CPU frequency to 333MHz
  ./aes $KEY $IV $INPUT $OUTPUT >> output_333_0.csv #append measurements to the file
  devmem2 0xF8000120 w 0x1F000800 #change the CPU frequency to 166MHz
  ./aes $KEY $IV $INPUT $OUTPUT >> output_166_0.csv #append measurements to the file
done
