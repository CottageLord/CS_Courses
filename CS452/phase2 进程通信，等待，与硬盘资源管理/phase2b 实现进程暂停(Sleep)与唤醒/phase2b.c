/******************************************
 * Class        : CSC-452 Phase 2 b
 * File         : phase2b.c 
 * Name         : Zehua Zhang / Yang Hu / Shu Lin
 * Description  : Use USLoss to handle process sleep
 ******************************************/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <usloss.h>
#include <phase1.h>
#include "phase2Int.h"

/* structure to record process sleep status */
typedef struct {
    int tag;
    int wakeTime;
} sleeper;

/* each process has a unique sleeper */
static sleeper sleepers[P1_MAXPROC];
/* each sleeper has a semaphore */
int timerSemaphore[P1_MAXPROC];
/* timer semaphore id */
int timer;

static int      ClockDriver(void *);
static void     SleepStub(USLOSS_Sysargs *sysargs);

/*
 * P2ClockInit
 *
 * Initialize the clock data structures and fork the clock driver.
 */
void 
P2ClockInit(void) 
{
    int rc;
    int pid;
    P2ProcInit();

    // initialize data structures here
    for(int i = 0; i < P1_MAXPROC; i ++){
        sleepers[i].tag = 0;
        sleepers[i].wakeTime = 0;
    }
    
    rc = P2_SetSyscallHandler(SYS_SLEEP, SleepStub);
    assert(rc == P1_SUCCESS);

    // fork the clock driver here
    rc = P1_Fork("ClockDriver", ClockDriver, NULL, USLOSS_MIN_STACK,2,0,&pid);
    assert(rc == P1_SUCCESS);
    
    rc = P1_SemCreate("timer",1 ,&timer);
    assert(rc == P1_SUCCESS);
}

/*
 * P2ClockShutdown
 *
 * Clean up the clock data structures and stop the clock driver.
 */
void 
P2ClockShutdown(void) 
{
    int rc,t;
    // get the parameter status of Wakup function
    rc = USLOSS_DeviceInput(USLOSS_CLOCK_DEV, 0 ,&t);
    assert(rc == P1_SUCCESS);
    
    // stop the clock driver, the last parameter indicates abort = TRUE
    rc =P1_WakeupDevice(USLOSS_CLOCK_DEV, 0, t, TRUE);
    assert(rc == P1_SUCCESS);
   
}
/*
 * ClockDriver
 *
 * Kernel process that manages the clock device and wakes sleeping processes.
 */
static int 
ClockDriver(void *arg) 
{
    int result;
    /* Enter kernel mode */
    result = USLOSS_PsrSet(USLOSS_PsrGet() | USLOSS_PSR_CURRENT_INT);
    assert(result == P1_SUCCESS);

    int rc;
    int now;

    while(1) {
        // wait for the next interrupt
        rc = P1_WaitDevice(USLOSS_CLOCK_DEV, 0, &now);
        if (rc == P1_WAIT_ABORTED) {
            break;
        }
        assert(rc == P1_SUCCESS);

        if(rc != USLOSS_DEV_OK){
            USLOSS_Halt(rc);
        }
        /* Create critical section*/
        rc = P1_P(timer);
        assert(rc == P1_SUCCESS);
        
        // wakeup any sleeping processes whose wakeup time has arrived
        for(int i = 0; i<P1_MAXPROC; i++){
            // get the current time
            int currentTime;
            rc = USLOSS_DeviceInput(USLOSS_CLOCK_DEV, 0 ,&currentTime);
            assert(rc == P1_SUCCESS);
            
            // wakeup a processes, and clear data
            if(sleepers[i].wakeTime <= currentTime && sleepers[i].tag != 0){
                rc = P1_V(timerSemaphore[i]);
                assert(rc == P1_SUCCESS);
                sleepers[i].wakeTime    = 0;
                sleepers[i].tag         = 0;
            }
        }
        rc = P1_V(timer);
        assert(rc == P1_SUCCESS);
    }
    return P1_SUCCESS;
}

/*
 * P2_Sleep
 *
 * Causes the current process to sleep for the specified number of seconds.
 */
int 
P2_Sleep(int seconds) 
{
    int rc, start, toSleep;
    P1_ProcInfo info;
    
    // Checks if in kernel mode
    unsigned int check = USLOSS_PsrGet();
    if((check & 1) == 0) {
         USLOSS_IllegalInstruction();
    }
    
    // check second is vaild
    if(seconds < 0 ){
        return P2_INVALID_SECONDS;
    }
    // get current process
    int pid = P1_GetPid();
    
    // get current time as sleeping start time
    rc = USLOSS_DeviceInput(USLOSS_CLOCK_DEV, 0, &start);
    if(rc != USLOSS_DEV_OK){
        USLOSS_Halt(rc);
    }
    
    rc = P1_GetProcInfo(pid, &info);
    assert(rc == P1_SUCCESS);
    
    rc = P1_SemCreate(info.name,0, &toSleep);
   
    assert(rc == P1_SUCCESS);
    
    rc = P1_P(timer);
    assert(rc == P1_SUCCESS);
    
    
    // add current process to an empty slot of data structure
    for(int i = 0; i < P1_MAXPROC; i++){
        if(sleepers[i].tag == 0){
            sleepers[i].tag = 1;
            sleepers[i].wakeTime =start + seconds * 1000000;
            timerSemaphore[i] = toSleep;
            break;
            
        }
    }

    rc = P1_V(timer);
    assert(rc == P1_SUCCESS);
    rc = P1_P(toSleep);
    assert(rc == P1_SUCCESS);
    rc = P1_SemFree(toSleep);
    assert(rc == P1_SUCCESS);
    
    return P1_SUCCESS;
}

/*
 * SleepStub
 *
 * Stub for the Sys_Sleep system call.
 */
static void 
SleepStub(USLOSS_Sysargs *sysargs) 
{
    int seconds = (int) sysargs->arg1;
    int rc = P2_Sleep(seconds);
    sysargs->arg4 = (void *) rc;
}

