#!/bin/bash
clear
gcc *.c ./yacsim.o -lm -o runme
./runme --numFUs 8 --numCopies 1 --numIterations 256 --trace 0 --numWaitBuffers 4