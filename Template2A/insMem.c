#include "global.h"


extern int NUM_ITERATIONS;
extern int TRACE;
extern unsigned insMEM[];
extern unsigned REG_FILE[];

void loadProgram() {
  int i;

  for (i=0; i < INSTR_MEM_SIZE; i++) 
      insMEM[i] =  0;  
  
  // PROGRAM  1
  // Accumulates the sum of an array of n consecutive memory words into Register R2
  // The base address of the array is in Register R1
  // Initialize R8 with the number of iterations n : Default is set at 5.
    
                  
//   insMEM[0] = 0x10200000;       // LOAD  R0, (R1)       top: R0 = MEM[R1];
//   insMEM[1] = 0x15094000;       // INTADD  R8, R8, R9         n = n - 1;
//   insMEM[2] = 0x04401000;       // ADDFP   R2, R2, R0         sum = sum + R0;
//   insMEM[3] = 0x14230800;       // INTADD  R1, R1, R3         R1 = R1 + 1;
//   insMEM[4] = 0x2500FFEC;       // BNEZ R8  -20               if (n != 0) gto top 
//   insMEM[5] = 0x28000000;       // HALT 
  
  
  // PROGRAM  2
  // Adds two arrays elemen-by-element and stores the result in a third array
  //  The base addresses of the source arrays are in Registers R2 and R4
  // The base address  of the destination array is R6
  // Initialize R8 with the number of iterations: Default is set at 5.
  
//   insMEM[0] = 0x15094000;       // INTADD  R8, R8, R9            top: n = n-1;
//   insMEM[1] = 0x10800800;       // LOAD R1, (R4)                    R1 = MEM[R4]
//   insMEM[2] = 0x10400000;       // LOAD R0, (R2)                    R0 = MEM[R2]
//   insMEM[3] = 0x04012800;       // ADDFP     R5, R0, R1             R5 = R0 + R1
//   insMEM[4] = 0x14431000;       // INTADD  R2, R2, R3               R2 = R2 + 1
//   insMEM[5] = 0x14832000;       // INTADD  R4, R4, R3               R4 = R4 + 1 
//   insMEM[6] = 0x18C50000;       // STORE R5, (R6)                   MEM[R6] = R5
//   insMEM[7] = 0x14C33000;       // INTADD  R6, R6, R3               R6 = R6 + 1
//   insMEM[8] = 0x2500FFDC;       // BNEZ R8  -36                     if (n != 0) goto top
//   insMEM[9] = 0x28000000;       //HALT 
  
  
  
  // PROGRAM  3   Array Copy
  // Copies words from one array to another
  // The base addresses of the source and destination arrays are in registers R1 and tR6 respectively.

 
  insMEM[0] = 0x10200000;       // LOAD   R0, 0(R1)      COPY: R0 = MEM[index];
  insMEM[1] = 0x18C00000;       // STORE  R0, (R6)             MEM[R6] = R0;   
  insMEM[2] = 0x15094000;       // INTADD  R8, R8, R9          count = count-1;
  insMEM[3] = 0x14230800;       // INTADD  R1, R1, R3          sourceIndex++;;
  insMEM[4] = 0x14C33000;       // INTADD  R6, R6, R3          destIndex++;
  insMEM[5] = 0x2500FFE8;       // BNEZ R8  -24                if (count!=0) goto COPY; 
  insMEM[6] = 0x28000000;       //HALT
}   

  


void loadRegFile() {  
  REG_FILE[0] = 0;  
  REG_FILE[1] = BASE_ARRAY;  
  REG_FILE[2] = BASE_ARRAY_SRC1;  
  REG_FILE[3] = 1;    
  REG_FILE[4] = BASE_ARRAY_SRC2;  
  REG_FILE[5] = BASE_ARRAY + NUM_ITERATIONS;  
  REG_FILE[6] = BASE_ARRAY_DEST;  
  REG_FILE[7] = 0;  
  REG_FILE[8] = NUM_ITERATIONS;
  REG_FILE[9] = -1;
}


