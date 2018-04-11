#!/bin/sh
# Runs 5 accelerated trials of AES encryption on the Parallela card for each of several different modes.
# (different FPGA and CPU frequencies)

KEY=00112233445566778899aabbccddeeff
IV=00112233445566778899aabbccddeeff
FILE=input16.txt
ITERS=5


for ((s = 4; s <= 12; ++s)) do
  devmem2 0xF8000170 w 0x100$(printf %x $s)00 > /dev/null #Change the FPGA frequency
  cat topfpga.bit > /dev/xdevcfg

  for ((i = 0; i < $ITERS; ++i)) do
    ./aes/client/client $KEY $IV $FILE
  done
done
