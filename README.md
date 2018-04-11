unix
======

C library of AES encryption based on the tiny-C-aes library, *aes.c* and *aes.h*. Folder also contains code to call the C library without acceleration to run on standard Unix.

Usage:

	make run


client
======

Performs accelerated AES encryption by talking to the FPGA accelerator over the NoC interface. Uses the same AES library as unaccelerated Unix.

Make sure the correct bitfile is loaded onto the silicon (using MD5 checksums of the bitfile using *md5sum*). Otherwise, the card will freeze when you try to run the program and you must reset it using bognor.

Usage:

	make run


fpga
======

Contains the Verilog code which implements the AES accelerator on the Zynq chip.

The output of running this code is a bitfile which needs to be downloaded onto the Parallela card (or whichever card was targeted) using *scp*:

	scp ./topfpga.bit root@parcard-djg1.sm:/home/dtc34

and then loaded onto the Zynq chip by typing the following as root:

	cat /home/dtc34/topfpga.bit > /dev/xdevcfg

If the image was successfully loaded, typing the following should return 1:

	cat /sys/devices/amba.0/f8007000.ps7-dev-cfg/prog_done


prazor-arm
======

Need to be change this file for the output work correctly: src/tenos/io_backdoor.cpp. Also need to change io_backdoor.h and armisa.cpp.

Specifically, make the following changes:

	2813 case 12: { /* fflush */
	2814   int i;
	2815
	2816   u32_t fbuf_addr = Reg(1);
	2817   int size = Reg(2);
	2818   char* buffer = (char*)malloc(sizeof(char)*size);
	2819
	2820   bzero(&buffer[0], size);
	2821   for (i = 0; i < size; ++i) {
	2822     armisa_read1(fbuf_addr++, 0, false, false);
	2823     char d = (char)read_data;
	2824     buffer[i] = d;
	2825   }
	2826
	2827   Reg(0) = io_backdoor_su->flush(Reg(0), buffer, size);
	2828   break;
	2829       }

	22   /* flush */
	23   int flush(int fid, char* buf, int size);
	24

	61 int io_backdoor_setup::flush(int fid, char* buf, int size) {
	62   FILE *f;
	63
	64   f = files[fid];
	65   fwrite(buf, size, 1, f);
	66
	67   return strlen(buf);
	68 }
	69

Prazor implementation of AES encryption without any acceleration. Uses the AES library from the Unix folder.  The key and IV are specified in the Makefile.

Usage:

First, set the PRAZOR environment variable to the root directory of your pre-compiled version of Prazor, i.e.

	export PRAZOR=/home/dtc34/Documents/P35/pvp11

Next, create a file *input.txt* in the prazor-arm folder. Then, type *make run*. The result of the encryption will be stored in output.txt.


prazor-accel
======

Prazor implementation with acceleration. Talks to 'module', which is the Prazor accelerator.

Usage:

Same as unaccelerated Prazor. Note that if you have not moved *module* to the correct place and followed the instructions below correctly, when you type *make run*, you will get caught in an infinite loop.


module
======

Prazor accelerator module. Must be manually copied into your Prazor directory:

	mkdir $PRAZOR/vhls/src/aes
	cp ./module/* $PRAZOR/vhls/src/aes/

Then, the following changes must be made to your copy of Prazor:

1. Include the two aes headers in the file zynq.h
2. The new block needs to be instantiated inside the zynq files src/platform/arm/zynq/parallella/zynq{.cpp,.h}
3. BUSMUX64_BIND(busmux0, aes0.port0, AES_BASE_ADDR, AES_SPACING); must be added to zynq.cpp to connect the device to the I/O bus.
4. src/Makefile.am needs to be adjusted to include the aes subdirectory. Make sure the aes subdirectory is listed at the beginning of the list, or at least before platform.
5. Change the variable AC_OUTPUT in configure.ac to include the AES directory.
6. add $(top_builddir)/src/aes/libaes.la to both libzynq_s_la_LIBADD and libzynq_la_LIBADD into src/platform/arm/zynq/parallella/Makefile.am


Whenever you are building Prazor, make sure you are in the vhls directory and type *autoreconf* followed by *./configure  TLM_POWER3=$TLM_POWER3 --with-tlm-power --with-speedo  --host=x86_64-pc-linux-gnu* ensuring all your environment variables are set properly.

To correctly measure power on Prazor and have the output go to nominal.power.txt, add POWER3_TRC_2(aes0, "AES") to ~line 567 in zynq.cpp.

test
======

Contains test input and output files to prove that the accelerator works on all the different platforms. .bin files are the raw output file before the hexdump, and .txt files are the output after the hexdump.


stats
======

Contains code to collect timing and energy statistics. Also contains example timing and energy logs from Nandor and Dom.

## Silicon
Whenever the encryption code is run, initial and final time-stamps/energy-stamps are taken and printed. These outputs should be piped into an output file named according to the configuration. 5 trials for each configuration should be run. The files should be named: *output_[CPU freq]_[FPGA_freq].txt* where FPGA_freq is 0 if we are running in unaccelerated mode. This would create the following output files:

	output_666_0.csv
	output_333_0.csv
	output_166_0.csv
	output_666_250.csv
	output_333_250.csv
	output_166_250.csv
	output_666_100.csv
	output_333_100.csv
	output_166_100.csv

These files will be produced by running *run_encryption_accel.sh* and *run_encryption_unaccel.sh*, which run the trials and produce the outputs automatically.

Measurements must also be taken to see how much power the silicon uses when it is not running encryption. This power will be an average based on the amount of energy measured a period of 60 seconds. The numbers will then be divided to find the average power of the idle Parallela board. This power will be compared to the power measured on the Parallela board.

###Old energy measuring method (no longer being used, only here for historical purposes)
Energy and timing measurements were taken from two separate files, making things complicated quite quickly. Time-of-day timestamps are used to get rid of energy measurements which don't take place during encryption. The typical amount of power which the parallela in FN12 uses in 100ms is about 32mJ.

*energy.py* collects energy information from the Parallela card running in FN12 (parcard-djg1.sm) over Telnet. It should be started just before you start executing the encryption code on the Parallela and stopped shortly after your code is finished executing (using Ctrl C). The *energy.py* output should be piped into a file *energy_%_$.log* where *%* is the ARM CPU frequency in MHz and $ is the frequency of the FPGA in MHz, 0 if unaccelerated. i.e. *energy_666_0.log* is for an ARM CPU of 666MHz without acceleration.

	./energy.py > energy_666_0.log

The same steps should be taken for the *timing_%_$.log* files for running multiple trials of the ARM execution time (alternatively, *encrypt_trials.sh* is used to run the encryption using various modes on the silicon Parallela card).

	./encrypt_trials.sh > timing_666_0.log

Then, configure the *process.py* with the frequency pair you want the stats for, and the python file will print out the stats.

## Prazor
*bench_time.sh* and *bench_power.sh* are used to collect timing and energy information from Prazor.


Changing frequencies
======
## Silicon
The devmem2 program (www.lartmaker.nl/lartware/port/devmem2.c) is used to read and write to the Zynq clock registers. devmem2 must be used as root. If you read and write an invalid value, you will kill the board and must reset it via bognor.

	devmem2 0xF8000120 w    // View current ARM CPU frequency
	devmem2 0xF8000170 w    // View current FPGA frequency

To change the silicon FPGA frequency (default divisor of 0xA):

	devmem2 0xF8000170 w 0x100400

Change the number 4 in 0x100400 to change what you divide the frequency by (clock normally runs at 1GHz).

To change the silicon ARM CPU frequency (666MHz by default):

	devmem2 0xF8000120 w 0x1F000200

In this case, 2 is the divisor, which must not be 0, 1 or 3. A value of 2 means a CPU frequency of 666MHz. See Zynq manual page 1583 for more info.

## Prazor

To change Prazor FPGA and CPU frequencies, pass in the frequencies as command line arguments. You must change src/vhls.cpp file (~line 235) to be able to do this.


To reset the parcard
======
Log into bognor.sm and type the following as root:

	parcard-djg1.reset


Misc
======
On Prazor,

To measure reads:

	./kiwi-ksubs3/kiwi-ksubs3-server/ksubs3.1-server -pio-performance -1000000

To measure writes:

	./kiwi-ksubs3/kiwi-ksubs3-server/ksubs3.1-server -pio-performance 1000000

where kiwi-ksubs3 is the repo originally at https://bitbucket.org/djg11/kiwi-ksubs3.git and now on Nandor's parcard account.
