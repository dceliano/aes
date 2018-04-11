#!/bin/bash
# Runs 5 accelerated trials of AES encryption on the Parallela card for each of several different modes.
# (different FPGA and CPU frequencies)
# Make sure the bitfile is loaded into the board.

KEY=00112233445566778899aabbccddeeff
IV=00112233445566778899aabbccddeeff
INPUT=plaintext
OUTPUT=output.txt
ITERS=5

function runiters {
	for ((i = 0; i < $ITERS; ++i)) do
		./client $KEY $IV $INPUT $OUTPUT >> $1
	done
}

#make empty files
> output_666_250.csv
> output_333_250.csv
> output_166_250.csv
> output_666_100.csv
> output_333_100.csv
> output_166_100.csv


devmem2 0xF8000170 w 0x100400 #Change FPGA frequency to 250MHz
devmem2 0xF8000120 w 0x1F000200 #change the CPU frequency to 666MHz
runiters output_666_250.csv
devmem2 0xF8000120 w 0x1F000400 #change the CPU frequency to 333MHz
runiters output_333_250.csv
devmem2 0xF8000120 w 0x1F000800 #change the CPU frequency to 166MHz
runiters output_166_250.csv

devmem2 0xF8000170 w 0x100A00 #Change FPGA frequency to 100MHz
devmem2 0xF8000120 w 0x1F000200 #change the CPU frequency to 666MHz
runiters output_666_100.csv
devmem2 0xF8000120 w 0x1F000400 #change the CPU frequency to 333MHz
runiters output_333_100.csv
devmem2 0xF8000120 w 0x1F000800 #change the CPU frequency to 166MHz
runiters output_166_100.csv
