#include "global.h"
#include <string.h>
#include <stdlib.h>

// IF Stage
unsigned insMEM[INSTR_MEM_SIZE];
unsigned PC, PC4;
unsigned INSTRUCTION;
unsigned nextPC;
int stallIF;
unsigned branchFlag;
int isHALT;

// Issue Stage
int scoreBoard[MAX_NUM_WAITBUFS][NUM_REGISTERS];
int pendingWrite[NUM_REGISTERS];
int pendingRead[NUM_REGISTERS];
struct WaitBufEntry  insWaitBuffer[MAX_NUM_WAITBUFS];
unsigned REG_FILE[NUM_REGISTERS];

// Dispatch Stage
int isFree[MAX_NUM_FU * MAX_NUM_COPIES];
unsigned  MEM[MEM_SIZE];
struct workEntry myWork[MAX_NUM_FU * MAX_NUM_COPIES];
int workAvail[MAX_NUM_FU * MAX_NUM_COPIES];

// Exec and Write Stages
SEMAPHORE * sem_myFU[MAX_NUM_FU * MAX_NUM_COPIES];
int resultReady[MAX_NUM_FU * MAX_NUM_COPIES];
struct resultEntry  resultData[MAX_NUM_FU * MAX_NUM_COPIES];


// Statistics
int numWAWStallCycles,numWARStallCycles;
int numInstrComplete;
int numStallCycles, numBranchStallCycles, numHaltStallCycles, numBranchDataStallCycles, numWaitBufferFullStallCycles;
int   numStallFUAvail[MAX_NUM_FU * MAX_NUM_COPIES];
double timeInsCompleted, timeInsRetired;

// Pipeline stage Processes
PROCESS *fetch, *issue, *dispatch, *execute, *write;
PROCESS  *display;
PROCESS *FU;

extern void reset();
extern   void fetchstage(), issuestage(), dispatchstage(), executestage(), writestage(), displaystage();
extern	void FUs();
extern void showRegFile();
extern void showWaitBuffer();

int NUM_WAITBUFS, NUM_FU, NUM_COPIES, NUM_ITERATIONS, TRACE;


void getparams(int argc, char *argv[]) {
  int i;

    for (i=1; i < argc ; i = i+2) {
      if (strcmp(argv[i], "--numIterations") == 0) {
	NUM_ITERATIONS = atoi(argv[i+1]);
      }
      else  if (strcmp(argv[i], "--numWaitBuffers") == 0){
	NUM_WAITBUFS = atoi(argv[i+1]);
      }
      else  if (strcmp(argv[i], "--numFUs") == 0) {
	NUM_FU = atoi(argv[i+1]);
      }
      else  if (strcmp(argv[i], "--numCopies") == 0){
	NUM_COPIES = atoi(argv[i+1]);
      }
      else  if (strcmp(argv[i], "--trace") == 0){
	TRACE  = atoi(argv[i+1]);
      }
      else {
	  printf("Unmatched Argument. BYE!!!\n");
	  exit(1);
      }
    }
}


void UserMain(int argc, char *argv[]){
  int i, j;

  getparams(argc, argv);
  printf("\n******************************************************************************\n");
  printf("Num_ITERATIONS: %d\tNUM_FUs: %d\tNUM_COPIES: %d\tNUM_WAITBUFS: %d TRACE: %s\n", NUM_ITERATIONS, NUM_FU, NUM_COPIES, NUM_WAITBUFS, TRACE? "ON" : "OFF");
    printf("******************************************************************************\n");

  reset();


	for (i=0; i < MAX_NUM_FU * MAX_NUM_COPIES; i++)
	   sem_myFU[i] = NewSemaphore("RSindex",0);

	numInstrComplete = 0;
	numStallCycles = 0;
	numWAWStallCycles = 0;
	numWARStallCycles = 0;     
	for (i=0; i < MAX_NUM_FU * MAX_NUM_COPIES; i++) 
	  numStallFUAvail[i] = 0;


	// create a process for each pipeline stage

		write = NewProcess("write",writestage,0);
		ActivitySetArg(write,NULL,1);
		ActivitySchedTime(write,0.000000,INDEPENDENT);

		execute = NewProcess("execute",executestage,0);
		ActivitySetArg(execute,NULL,1);
		ActivitySchedTime(execute,0.000001,INDEPENDENT);


		dispatch = NewProcess("dispatch",dispatchstage,0);
		ActivitySetArg(dispatch,NULL,1);
		ActivitySchedTime(dispatch,0.00002,INDEPENDENT);


		issue = NewProcess("issue",issuestage,0);
		ActivitySetArg(issue,NULL,1);
		ActivitySchedTime(issue,0.00003,INDEPENDENT);


		fetch = NewProcess("fetch",fetchstage,0);
		ActivitySetArg(fetch,NULL,1);
		ActivitySchedTime(fetch,0.00004,INDEPENDENT);


		for (j=0; j < NUM_COPIES; j++) {
		  for (i=1; i < NUM_FU; i++) {
		    FU = NewProcess("fui", FUs,0);
		    ActivitySetArg(FU,NULL,j*NUM_FU + i);
		    ActivitySchedTime(FU,0.00003,INDEPENDENT);
		  }
		}

		// create a display process to print out pipeline state

		  display = NewProcess("display",displaystage,0);
		  ActivitySetArg(display,NULL,1);
		  ActivitySchedTime(display,0.00004,INDEPENDENT);


// Initialization is done, now start the simulation


	DriverRun(MAX_SIMULATION_TIME);

	printf("\n\n****************************************************************************************\n");
	printf("Simulation ended  at %3.0f\n",GetSimTime());
	printf("NUM_WAITBUFS: %d NUM_COPIES: %d  LOAD_CYCLES: %d\n", NUM_WAITBUFS, NUM_COPIES, LOAD_CYCLES);
	printf("Execution completed at time  %3.0f\n",timeInsCompleted);
	printf("Retirement completed  at time  %3.0f\n",timeInsRetired);

	printf("Number Instructions Retired: %d\n", numInstrComplete);
	printf("Number Stall Cycles: %d\n", numStallCycles);
	printf("Number Stall Cycles: HALT : %d WaitBufferFull : %d BranchControl: %d  Branch Data: %d\n", numHaltStallCycles, numWaitBufferFullStallCycles, numBranchStallCycles, numBranchDataStallCycles);

	for (i=0; i < NUM_FU * NUM_COPIES; i++) {
	  if (numStallFUAvail[i] > 0)
	    printf("Number Stall Cycles FU[%d]: %d\n", i, numStallFUAvail[i]);
	}
	printf("Number WAW Stall Cycles: %d\n", numWAWStallCycles);
	printf("Number WAR Stall Cycles: %d\n", numWARStallCycles);

	showRegFile();

	// Uncommented if you want to see destination array in Program 2
	// Change it to print Every 64th MEM value when you test with a large data size.

		  printf("\nFinal Destination Memory Array\n");
		  
		   for (i=BASE_ARRAY_DEST; i < BASE_ARRAY_DEST + NUM_ITERATIONS ; i = i + 1)
			 			  printf("MEM[%d]: %d\n", i, MEM[i]);
		  		  
}


