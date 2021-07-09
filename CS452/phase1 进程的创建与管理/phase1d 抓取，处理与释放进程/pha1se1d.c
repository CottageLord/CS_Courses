#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "usloss.h"
#include "phase1.h"
#include "phase1Int.h"

static void DeviceHandler(int type, void *arg);
static void SyscallHandler(int type, void *arg);
static void IllegalInstructionHandler(int type, void *arg);
static int sentinel(void *arg);

/* if user mode (0 at 0x1) return true */
#define checkMode() 
    if(!(USLOSS_PsrGet() & USLOSS_PSR_CURRENT_MODE)){\
        USLOSS_Console("Error Mode.\n");\
        USLOSS_IllegalInstruction();}

/* the MACRO restoreInt relies on disableInt on variable wasEnabled */
#define disableInt() \
        int wasEnabled = (USLOSS_PsrGet() & 0x2) >> 1;\
        checkReturn(USLOSS_PsrSet(USLOSS_PsrGet() & ~0x2));

#define restoreInt() \
        if(wasEnabled) checkReturn(USLOSS_PsrSet(USLOSS_PsrGet() | 0x2));

int deviceAbort = 0;

void 
startup(int argc, char **argv)
{
    checkMode();
    int pid;
    P1SemInit();

    // TODO : initialize device data structures
    
    // put device interrupt handlers into interrupt vector
    USLOSS_IntVec[USLOSS_SYSCALL_INT] = SyscallHandler;
    USLOSS_IntVec[USLOSS_ILLEGAL_INT] = IllegalInstructionHandler;

    /* create the sentinel process */
    int rc = P1_Fork("sentinel", sentinel, NULL, USLOSS_MIN_STACK, 6 , 0, &pid);
    assert(rc == P1_SUCCESS);
    // should not return
    assert(0);
    return;

} /* End of startup */

int 
P1_WaitDevice(int type, int unit, int *status) 
{
    checkMode();
    int result = *status;
    // P device's semaphore
    P1_P(unit);
    // TODO : set *status to device's status
    return result;
}

int P1CheckType (int type) {
    // if the type is one of the 4 divice types, return valid
    if(type == USLOSS_CLOCK_INT || type == USLOSS_ALARM_INT ||
        type == USLOSS_DISK_INT || type == USLOSS_TERM_INT) {
        return P1_SUCCESS;
    }
    return P1_INVALID_TYPE;
}

int P1CheckUnit (int unit) {
    // if the unit is one of the 4 divice units, return valid
    if(unit == USLOSS_CLOCK_UNITS || unit == USLOSS_ALARM_UNITS ||
        unit == USLOSS_DISK_UNITS || unit == USLOSS_TERM_UNITS) {
        return P1_SUCCESS;
    }
    return P1_INVALID_UNIT;
}

int 
P1_WakeupDevice(int type, int unit, int status, int abort) 
{
    checkMode();
    int result = P1_SUCCESS;
    // TODO : save device's status
    // int savedStatus = status;
    // V device's semaphore
    P1_V(unit);
    return result;
}

static void
DeviceHandler(int type, void *arg) 
{
    // TODO : 
    // if clock device
    //      P1_WakeupDevice every 4 ticks
    //      P1Dispatch(TRUE) every 5 ticks
    // else
    //      P1_WakeupDevice
}

static int
sentinel (void *notused)
{
    // check for kernel mode
    checkMode();

    int     pid;
    int     rc;

    /* start the P2_Startup process */
    rc = P1_Fork("P2_Startup", P2_Startup, NULL, 4 * USLOSS_MIN_STACK, 2 , 0, &pid);
    assert(rc == P1_SUCCESS);

    // TODO : 
    // enable interrupts
    // while sentinel has children
    //      get children that have quit via P1GetChildStatus
    //      wait for an interrupt via USLOSS_WaitInt
    USLOSS_Console("Sentinel quitting.\n");
    return 0;
} /* End of sentinel */

int 
P1_Join(int tag, int *pid, int *status) 
{
    // check for kernel mode
    checkMode();
    int result = P1_SUCCESS;
    // TODO : 
    // disable interrupts
    // do
    //  get a child that has quit via P1GetChildStatus 
    //  if no children have quit
    //      set state to P1_STATE_JOINING vi P1SetState
    //      P1Dispatch(FALSE)
    // until either a child quit or there are no more children
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
