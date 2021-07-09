/******************************************
 * Class        : CSC-452 Phase 2 a
 * File         : phase2a.c 
 * Name         : Yang Hu / Shu Lin
 * Description  : Use USLoss to handle system calls
 ******************************************/
#include <stdlib.h>
#include <usloss.h>
#include <phase1.h>
#include <phase2.h>
#include <string.h>

#include <stdio.h>
#include <assert.h>
#include <libuser.h>
#include <usyscall.h>
#include "phase2Int.h"

#define TAG_KERNEL 0
#define TAG_USER 1

/* Processor Status Register
    8       4      0
    |-------|-------|
    | | | | |_|_|_|_|
    |--------|-|-|-||
             | | | |____Current Mode (1-kernel, 0-user)
             | | |______Current Interrupt Enable (1-enabled)
             | |________Previous Mode
             |__________Previous Interrupt Enable
 */

typedef struct wrapperStruct
{
    char name[20];
    int priority;
    int stackSize;
    int (*function)(void *);   /* Starting function */
    void *funcArg;             /* Arg to starting function */
} wrapperStruct;

void (*syscalls[USLOSS_MAX_SYSCALLS]) (USLOSS_Sysargs *sysargs);
/* Stub functions */
static void SpawnStub(USLOSS_Sysargs *sysargs);
static void WaitStub(USLOSS_Sysargs *sysargs);
static void ProcInfoStub(USLOSS_Sysargs *sysargs);
static void GetPIDStub(USLOSS_Sysargs *sysargs);
static void GetTimeOfDay(USLOSS_Sysargs *sysargs);

/* if user mode (0 at 0x1) call IllegalHandler */
#define MODECHECK {\
    if (!(USLOSS_PsrGet() & USLOSS_PSR_CURRENT_MODE)) {\
        USLOSS_Console("ERROR: usermode cannot execute syscall\n");\
        IllegalHandler(USLOSS_ILLEGAL_INT, NULL);\
    }\
}

/* if failed exception return true*/
#define checkReturn(x) \
    if( x == USLOSS_ERR_INVALID_PSR ) {\
        USLOSS_Console("Invalid PSR value.");\
        USLOSS_Halt(1);}

#define CHECKSETRET(x) \
    if( x == USLOSS_ERR_INVALID_PSR ) {\
        USLOSS_Console("ERROR: Invalid PSR value.");\
        USLOSS_Halt(1);}

/*
 * IllegalHandler
 * Handler for illegal instruction interrupts.
 */

static void 
IllegalHandler(int type, void *arg) 
{
    P1_ProcInfo info;
    assert(type == USLOSS_ILLEGAL_INT);
    int pid = P1_GetPid();
    int rc = P1_GetProcInfo(pid, &info);
    assert(rc == P1_SUCCESS);
    if (info.tag == TAG_KERNEL) {
        USLOSS_Console("ILLEGAL 1024\n");
        P1_Quit(1024);
    } else {
        USLOSS_Console("ILLEGAL 2048\n");
        P2_Terminate(2048);
    }
}

/*
 * SyscallHandler
 * Handler for system call interrupts.
 */
static void 
SyscallHandler(int type, void *arg)
{
    // enter kernel mode
    checkReturn(USLOSS_PsrSet(USLOSS_PsrGet() | 0x1)); 
    //USLOSS_Console("Entering handler...\n");
	// extract argument(s)
	USLOSS_Sysargs *sysargs = (USLOSS_Sysargs *) arg;
	int syscall = sysargs->number;

    //USLOSS_Console("TYPE is %d\n syscall is %d\n", type, syscall);
    // if invalid syscall
    if(syscall < 0 || syscall > USLOSS_MAX_SYSCALLS){
        sysargs->arg4 = (void*) P2_INVALID_SYSCALL;
        return;
    }
	// enable interrupt
	checkReturn(USLOSS_PsrSet(USLOSS_PsrGet() | USLOSS_PSR_CURRENT_INT));
    // call handler
    if(syscalls[syscall] != NULL) (*syscalls[syscall]) (sysargs);
}


/*
 * P2ProcInit
 *
 * Initialize everything.
 *
 */

void
P2ProcInit(void) 
{
    // enter kernel mode
    checkReturn(USLOSS_PsrSet(USLOSS_PsrGet() | 0x1)); 
    int rc;
    USLOSS_IntVec[USLOSS_ILLEGAL_INT] = IllegalHandler;
    USLOSS_IntVec[USLOSS_SYSCALL_INT] = SyscallHandler;
    // call P2_SetSyscallHandler to set handlers for all system calls
    rc = P2_SetSyscallHandler(SYS_SPAWN, SpawnStub);
    rc = P2_SetSyscallHandler(SYS_WAIT, WaitStub);
    rc = P2_SetSyscallHandler(SYS_TERMINATE, ProcInfoStub);
    rc = P2_SetSyscallHandler(SYS_GETPID, GetPIDStub);
    rc = P2_SetSyscallHandler(SYS_GETTIMEOFDAY, GetTimeOfDay);
    assert(rc == P1_SUCCESS);
    // enter usermode
    // checkReturn(USLOSS_PsrSet((USLOSS_PsrGet() >> 1) << 1));
}

/*
 * P2_SetSyscallHandler
 * Set the system call handler for the specified system call.
 */

int
P2_SetSyscallHandler(unsigned int number, void (*handler)(USLOSS_Sysargs *args))
{
    syscalls[number] = handler;
    return P1_SUCCESS;
}

int wrapper(void *arg)
{
    // Check kernel mode
    MODECHECK;
    // unpack the original info
    wrapperStruct *tmp = (wrapperStruct *)arg;
    
    // Enable interrupts
    checkReturn( USLOSS_PsrSet(USLOSS_PsrGet() | USLOSS_PSR_CURRENT_INT));
    
    // enter usermode
    checkReturn(USLOSS_PsrSet((USLOSS_PsrGet() >> 1) << 1));
    // call the original function
    int rc = tmp->function(tmp->funcArg);
    
    Sys_Terminate(rc);
    return 0;
}

/*
 * P2_Spawn
 * Spawn a user-level process.
 */
int 
P2_Spawn(char *name, int(*func)(void *arg), void *arg, int stackSize, int priority, int *pid) 
{
	MODECHECK;

    wrapperStruct *newWrapper = (wrapperStruct *)malloc(sizeof(wrapperStruct));
    // pass all related info into wrapper
    strcpy(newWrapper->name, name);
    newWrapper->priority = priority;
    newWrapper->stackSize = stackSize;
    newWrapper->function = func;
    newWrapper->funcArg = arg;
    //USLOSS_Console("* Forking: %s(%d) with stackSize %d\n", newWrapper->name, newWrapper->priority, newWrapper->stackSize);
    int retVal = P1_Fork(newWrapper->name, wrapper, newWrapper, stackSize, newWrapper->priority, TAG_USER, pid);
    //USLOSS_Console("fork result: %d\n", retVal);
    return retVal;
}

/*
 * P2_Wait
 * Wait for a user-level process.
 */

int 
P2_Wait(int *pid, int *status) 
{
	MODECHECK;
    int retVal = P1_Join(TAG_USER, pid, status);
    return retVal;
}

/*
 * P2_Terminate
 * Terminate a user-level process.
 */

void 
P2_Terminate(int status)
{
	MODECHECK;
    P1_Quit(status);
}

/*
 * SpawnStub
 * Create a user-level process (this is the user-level equivalent of P1_Fork). If the
 * spawned function returns it has the same effect as calling Sys_Terminate.
 */

static void 
SpawnStub(USLOSS_Sysargs *sysargs) 
{
    MODECHECK;
	/*
     Input:
     arg1: address of the function to spawn
     arg2: parameter passed to spawned function
     arg3: stack size (in bytes)
     arg4: priority
     arg5: character string containing process’s name
     
     Output:
     arg1: the PID of the newly created process; -1 if a process could not be created
     */
	
    int (*func)(void *) = sysargs->arg1;
    void *arg = sysargs->arg2;
    int stackSize = (int) sysargs->arg3;
    int priority = (int) sysargs->arg4;
    char *name = sysargs->arg5;
    int pid;
    int rc = P2_Spawn(name, func, arg, stackSize, priority, &pid);
    if (rc == P1_SUCCESS) {
        sysargs->arg1 = (void *) pid;
    }
    sysargs->arg4 = (void *) rc;
}

/* WaitStub
 * Wait for a child process created via Sys_Spawn to terminate (user-level equivalent of
 * P1_Join).
 */
static void
WaitStub(USLOSS_Sysargs *sysargs)
{
    MODECHECK;
	/*
     Output:
     arg1: process id of the terminating child.
     arg2: the termination code of the child
     arg4: -1 if the process has no children, 0 otherwise
     */
	int tcode;
	int pid;
	int retVal = P2_Wait(&pid, &tcode);
	// if success
    if(retVal > 0)
    {
    	sysargs->arg1 = (void *) pid;
        sysargs->arg2 = (void *) tcode;
        sysargs->arg4 = (void *) 0;
    }
    // no children
    else
    {
        sysargs->arg4 = (void *) -1;
    }
}

/* ProcInfoStub
 * Returns process information (P1_GetProcInfo)
 */

static void 
ProcInfoStub(USLOSS_Sysargs *sysargs)
{
    MODECHECK;
    /*Input
        arg1: PID of the process
        arg2: pointer to P1_ProcInfo to be filled in
    Output
        arg4: return code from P1_GetProcInfo*/
    sysargs->arg4 = (void *) P1_GetProcInfo((int) sysargs->arg1, (P1_ProcInfo *) sysargs->arg2);

}

/* GetPIDStub
 * Returns the process ID of the currently running process (P1_GetPID).
 */
static void
GetPIDStub(USLOSS_Sysargs *sysargs)
{
    MODECHECK;
    /*Output
        arg1: the process’s PID*/
    sysargs->arg1 = (void *) P1_GetPid();
}

/* GetTimeOfDay
 * Returns the value of the USLOSS clock.
 */

static void
GetTimeOfDay(USLOSS_Sysargs *sysargs)
{
    MODECHECK;
    /*Output
        arg1: clock value*/
    int time;
    // unit must < USLOSS_CLOCK_UNITS, in this case <1, which is 0
    if( USLOSS_DeviceInput(USLOSS_CLOCK_DEV, 0, &time) == USLOSS_DEV_OK)
    {
        sysargs->arg1 = (void *) time;
    }
}