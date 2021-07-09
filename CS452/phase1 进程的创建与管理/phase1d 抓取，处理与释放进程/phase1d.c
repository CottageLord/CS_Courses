/******************************************
 * Class        : CSC-452 Phase 1 d
 * File         : phase1d.c
 * Name         : Yang Hu / Shu Lin
 * Description  : Use USLoss to simulate simple os (Process Management functionalities)
 ******************************************/
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "usloss.h"
#include "phase1.h"
#include "phase1Int.h"


/* if failed exception return true*/
#define checkReturn(x) {\
    if( x == USLOSS_ERR_INVALID_PSR ) {\
        USLOSS_Console("Invalid PSR value.\n");\
        USLOSS_Halt(1);}}

/* if user mode (0 at 0x1) return true */
#define checkMode() {\
    if(!(USLOSS_PsrGet() & USLOSS_PSR_CURRENT_MODE)){\
        USLOSS_Console("Error Mode.\n");\
        USLOSS_IllegalInstruction();}}

#define disableInt() \
        checkReturn(USLOSS_PsrSet(USLOSS_PsrGet() & ~0x2));

#define restoreInt() \
        checkReturn(USLOSS_PsrSet(USLOSS_PsrGet() | 0x2));

static void DeviceHandler(int type, void *arg);
static void SyscallHandler(int type, void *arg);
static void IllegalInstructionHandler(int type, void *arg);
static int sentinel(void *arg);

// r = types, c = units
int *arr[4];
int deviceAbort, deviceStatus, tickCount;
void 
startup(int argc, char **argv)
{
    int pid;
    P1SemInit();

    // initialize device data structures
    // USLOSS_CLOCK_DEV 1 units
    arr[0] = malloc(USLOSS_CLOCK_UNITS * sizeof(int));
    // USLOSS_ALARM_DEV 1 units
    arr[1] = malloc(USLOSS_ALARM_UNITS * sizeof(int));
    // USLOSS_DISK_DEV 2 units
    arr[2] = malloc(USLOSS_DISK_UNITS * sizeof(int));
    // USLOSS_TERM_DEV 4 units
    arr[3] = malloc(USLOSS_TERM_UNITS * sizeof(int));

    int semCreateResult = 0;
    tickCount = 0;
    for (int i = 0; i < 4; ++i)
    {
        int currSize = sizeof(arr[i]) / sizeof(int);

        for(int j = 0; j < currSize; j++) {
            // i j and \0
            char *name = malloc(sizeof(char) * 3);
            sprintf(name, "%d%d", i, j);
            semCreateResult = P1_SemCreate(name, 0, &arr[i][j]);
            if(semCreateResult != P1_SUCCESS) P1_Quit(1024);
        }
    }
    USLOSS_IntVec[USLOSS_SYSCALL_INT] = SyscallHandler;
    USLOSS_IntVec[USLOSS_ILLEGAL_INT] = IllegalInstructionHandler;
    // put device interrupt handlers into interrupt vector
    USLOSS_IntVec[USLOSS_CLOCK_INT]   = DeviceHandler;

    /* create the sentinel process */
    int rc = P1_Fork("sentinel", sentinel, NULL, USLOSS_MIN_STACK, 6 , 0, &pid);
    assert(rc == P1_SUCCESS);
    // should not return
    assert(0);
    return;

} /* End of startup */

int P1CheckType (int type) {
    // if the type is one of the 4 divice types, return valid
    if(type == USLOSS_CLOCK_INT || type == USLOSS_ALARM_INT ||
        type == USLOSS_DISK_INT || type == USLOSS_TERM_INT) {
        return P1_SUCCESS;
    }
    return P1_INVALID_TYPE;
}

int P1CheckUnit (int type, int unit) {
    // if the unit is one of the 4 divice units, and it matches with type, return valid
    if((type == USLOSS_CLOCK_DEV && unit <= USLOSS_CLOCK_UNITS && unit >= 0) || 
        (type == USLOSS_ALARM_DEV && unit <= USLOSS_ALARM_UNITS && unit >= 0) ||
        (type == USLOSS_DISK_DEV && unit <= USLOSS_DISK_UNITS && unit >= 0) || 
        (type == USLOSS_TERM_DEV && unit <= USLOSS_TERM_UNITS && unit >= 0)) {
        return P1_SUCCESS;
    }
    return P1_INVALID_UNIT;
}

int 
P1_WaitDevice(int type, int unit, int *status) 
{
    int result = P1_SUCCESS;
    int Presult;
    // disable interrupts
    disableInt();
    // check kernel mode
    checkMode();
    // P device's semaphore
    if(P1CheckType(type) != P1_SUCCESS) return P1_INVALID_TYPE;
    if(P1CheckUnit(type, unit) != P1_SUCCESS) return P1_INVALID_UNIT;

    Presult = P1_P(arr[type][unit]);
    if(Presult != P1_SUCCESS) P1_Quit(1024);
    // set *status to device's status
    *status = deviceStatus;
    // restore interrupts
    restoreInt();
    return result;
}

int 
P1_WakeupDevice(int type, int unit, int status, int abort) 
{
    int result = P1_SUCCESS;
    int Vresult;
    // disable interrupts
    disableInt();
    // check kernel mode
    checkMode();
    // save device's status to be used by P1_WaitDevice
    deviceStatus = status;
    // save abort to be used by P1_WaitDevice
    deviceAbort = abort;
    // V device's semaphore
    if(P1CheckType(type) != P1_SUCCESS)return P1_INVALID_TYPE;
    if(P1CheckUnit(type, unit) != P1_SUCCESS)return P1_INVALID_UNIT;
    Vresult = P1_V(arr[type][unit]);
    if(Vresult != P1_SUCCESS) P1_Quit(1024);
    // restore interrupts
    restoreInt();
    return result;
}

static void
DeviceHandler(int type, void *arg) 
{
    // if clock device
    if(type == USLOSS_CLOCK_DEV) {
        // P1_WakeupDevice every 5 ticks
        if(tickCount % 5 == 0){
            if(USLOSS_DeviceInput(type, (int) arg, &deviceStatus) != P1_SUCCESS) {
                P1_Quit(1024);
            }
            if(P1_WakeupDevice(type, (int) arg, deviceStatus, FALSE) != P1_SUCCESS) {
                P1_Quit(1024);
            }
        }
        // P1Dispatch(TRUE) every 4 ticks
        if(tickCount % 4 == 0) {
            P1Dispatch(TRUE);
        }
    } else {
        if(USLOSS_DeviceInput(type, (int) arg, &deviceStatus) != P1_SUCCESS) {
            P1_Quit(1024);
        }
        if(P1_WakeupDevice(type, (int) arg, deviceStatus, FALSE) != P1_SUCCESS) {
            P1_Quit(1024);
        }
    }
    tickCount++;
}

static int
sentinel (void *notused)
{
    int     pid;
    int     rc;

    /* start the P2_Startup process */
    rc = P1_Fork("P2_Startup", P2_Startup, NULL, 4 * USLOSS_MIN_STACK, 2 , 0, &pid);
    assert(rc == P1_SUCCESS);
    // enable interrupts
    checkReturn(USLOSS_PsrSet(USLOSS_PsrGet() | 0x2));
    int childPid;
    int status;
    // while sentinel has children
    // get children that have quit via P1GetChildStatus (either tag)
    P1GetChildStatus(0, &childPid, &status);
    while (status != P1_NO_CHILDREN) {
        // wait for an interrupt via USLOSS_WaitInt
        USLOSS_WaitInt();
        P1GetChildStatus(0, &childPid, &status);
    }
    return 0;
} /* End of sentinel */

int 
P1_Join(int tag, int *pid, int *status) 
{
    int result = P1_SUCCESS;
    // disable interrupts
    disableInt();
    // kernel mode
    checkMode();
    int childrenStatus;
    do {
        // use P1GetChildStatus to get a child that has quit  
        childrenStatus = P1GetChildStatus(tag, pid, status);
        // if no children have quit
        if(childrenStatus == P1_NO_QUIT) {
            // set state to P1_STATE_JOINING via P1SetState
            P1SetState(P1_GetPid(), P1_STATE_JOINING, 0);
            P1Dispatch(FALSE);
        }
    // until either a child quit or there are no more children
    } while(childrenStatus != P1_NO_CHILDREN && childrenStatus != P1_SUCCESS);
    return result;
}

static void
IllegalInstructionHandler(int type, void *arg)
{
    checkMode();
    P1_Quit(1024); // quit process with arbitrary status 1024
}


static void
SyscallHandler(int type, void *arg) 
{
    checkMode();
    USLOSS_Console("System call %d not implemented.\n", (int) arg);
    USLOSS_IllegalInstruction();
}

void finish(int argc, char **argv) {}
