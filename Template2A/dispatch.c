#include "global.h"

extern struct WaitBufEntry  insWaitBuffer[]; // From ISSUE stage
extern unsigned REG_FILE[ ];
extern int scoreBoard[][NUM_REGISTERS]; 
extern int pendingWrite[], pendingRead[];
extern int isFree[];  // Set in   EXECUTE and WRITE stages
extern int workAvail[ ];   // To EXEC stge 
extern struct workEntry  myWork[ ]; // To EXEC Stage

extern int   numStallFUAvail[]; // Statistics
extern int NUM_WAITBUFS,  TRACE;
extern char * map(int);

void  do_dispatch();

/****************************************************************************** */

void updatePendingReads(int srcReg) {
  int i;

  pendingRead[srcReg] = FALSE;
  for (i=0; i < NUM_WAITBUFS; i++) {
    if (scoreBoard[i][srcReg] == TRUE) {
      pendingRead[srcReg] = TRUE;
      return;
    }
  }
}

void dispatchstage() {  
  int job_num;
  job_num = ActivityArgSize(ME) - 1;
  while(TRUE){	  
    if (TRACE)
      printf("In DISPATCH Stage at time %2.0f\n", GetSimTime());
    do_dispatch();
    ProcessDelay(1.000);
  }
}

void do_dispatch() {
  int i, j, index;
  int fu;
 int srcReg1, srcReg2, destReg;
 static int next = 0;


 for (i = next; i < next + NUM_WAITBUFS; i=i+1) {
   if (insWaitBuffer[i % NUM_WAITBUFS].free == TRUE)
     continue;  // No instruction in this slot

  // Found an entry in WaitBuffer with a waiting instruction  
   index = i % NUM_WAITBUFS;
   srcReg1 = insWaitBuffer[index].srcReg1;
   srcReg2 =  insWaitBuffer[index].srcReg2;
   destReg = insWaitBuffer[index].destReg;

   if  ( pendingWrite[srcReg1] == TRUE  &&  insWaitBuffer[index].op1Ready == FALSE)  
     continue;  // RAW dependency on an in-flight instruction
   if (srcReg2 != -1)
     if ( pendingWrite[srcReg2] == TRUE &&  insWaitBuffer[index].op2Ready == FALSE )
       continue; // RAW dependency on an in-flight instruction

   // Current instruction has its RAW dependencies satisfed. 
  
   fu = insWaitBuffer[index].fu;
   if  (!isFree[fu]) {
     numStallFUAvail[fu]++;  // Statistics counter
     if (TRACE)
       printf(" FU (%s)  unit not available  numStall: %d Time: %5.2f\n", map(fu), numStallFUAvail[fu], GetSimTime()); 
     continue;  // FU not free 
   }
 
 // Operands and FU available
   myWork[fu].operand1 = REG_FILE[srcReg1];
   if (srcReg2 !=  -1) 
     myWork[fu].operand2 = REG_FILE[srcReg2];
   myWork[fu].destReg = destReg;
   workAvail[fu] = TRUE; // Signal FU 
   	
   if (DEBUG2) {
     printf("\n**********************************\n");
     printf("operand1: %d  operand2: %d    destReg: %d\n", myWork[fu].operand1, myWork[fu].operand2, myWork[fu].destReg);
     printf("\n**********************************\n");
   }
   
    scoreBoard[index][srcReg1] = FALSE;  
    updatePendingReads(srcReg1);
    if (srcReg2 != -1) {
      scoreBoard[index][srcReg2] = FALSE;
      updatePendingReads(srcReg2);
    }
   
   
    if (TRACE) 
      printf("\tActivating FU %s from insWaitBuffer[%d] at time %5.2f\n", map(insWaitBuffer[index].fu), index, GetSimTime());
      
    insWaitBuffer[index].free = TRUE;
    // Why not reset the fields of the insWaitBuffer slot for safety.
    next = (index+1) % NUM_WAITBUFS;

    if (DEBUG2)
      printf("Freeing up WaitBuffer entry %d\n", index);
    
    return;  // At most 1 instruction dispatched in any cycle
  }

 if (TRACE)
   printf("\tCould not find any instruction to dispatch. Time: %5.2f\n", GetSimTime());
}
