#include <math.h>
#include <stdio.h>
#include "sim.h"

#define epsilon 0.001
#define MAX_SIMULATION_TIME   80000.0 // 80000.0 (for experiments) 
#define TRUE 1
#define FALSE 0
#define DEBUG FALSE
#define DEBUG2  FALSE

#define NOP 0
#define ADDFP 1
#define SUBFP 2
#define MULFP 3
#define LOAD 4
#define INTADD 5
#define STORE 6

#define BRANCH 8
#define BNEZ 9
#define HALT 10

#define ADDFP_CYCLES  100 // 100 (for experiments) 
#define SUBFP_CYCLES  4
#define MULFP_CYCLES  8
#define LOAD_CYCLES   100 //100 (for experiments)
#define INTADD_CYCLES 1
#define STORE_CYCLES  100 // 100 (for experiments) 

#define MAX_NUM_WAITBUFS 256
#define MAX_NUM_FU 32
#define MAX_NUM_COPIES 16 

#define NUM_REGISTERS 16
#define MEM_SIZE  1024
#define INSTR_MEM_SIZE 1024

#define MAX_NUM_ITERATIONS 2048
#define BASE_ARRAY 0
#define BASE_ARRAY_SRC1 0
#define BASE_ARRAY_SRC2 256 
#define BASE_ARRAY_DEST 512 

struct WaitBufEntry {
  int free;
  int fu;
  int  op1Ready, op2Ready;
  unsigned  srcReg1, srcReg2, destReg;
};

struct workEntry {
  unsigned  operand1, operand2, destReg;
};


struct resultEntry {
  unsigned  result, destReg;
};
