/**
 * phase2.c
 * Part A
 *
 * @author: Hercy (Yunhao Zhang)
 * @author: David Carrig
 *
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libuser.h>
#include <phase1.h>
#include <phase2.h>
#include <usloss.h>

/* DEBUG FLAGS */
#define DEBUG_A         0
#define DEBUG_A_DETAIL  0


#define INVALID        -1

static int i;

typedef struct processBlock
{
    char name[20];
    int priority;
    int stackSize;
    int (*startFunc)(void *);   /* Starting function */
    void *startArg;             /* Arg to starting function */
} processBlock;

static void enterUserMode();
//static void launch(void *arg);
static int launch(void *arg);
static void syscall_handler(int type, void *offset);
static int inKernelMode();


/* --------------------------------------------------
 *  Clock Driver Stuff
 * -------------------------------------------------- */
static int ClockDriver(void *arg);
static void printTimers();
typedef struct timer
{
    int PID;
    int timeStarted;
    int secondsWait;
    int timeWaited;
} timer;
timer timeMachine[P1_MAXPROC];  /* Each process has a unique timer */
P1_Semaphore timerMutex, timerSemaphore[P1_MAXPROC];





/* --------------------------------------------------
 *  Mail Box & Messgae Stuff
 * -------------------------------------------------- */
typedef struct message
{
    int messageSize;
    //int messageID;
    //char * message;
    void * message;
    struct message * nextMessage;
} message;

typedef struct MBox
{
    int MBoxID; // Start from 1 NOT 0
    message * messageList;
    int size;
    int slots;
    int slotsUsed;
    
} MBox;

MBox mailBoxList[P2_MAX_MBOX];  // List of mail boxs
P1_Semaphore mailBoxMutex, MBoxFull[P2_MAX_MBOX], MBoxEmpty[P2_MAX_MBOX];

static void insert(message **list, void * msg, int size);




/* --------------------------------------------------
 *  P2_MboxCreate()
 * --------------------------------------------------
 *  return:>=0  return mail box ID
 *          -1  invalid
 *          -2  illegal values given
 *          -3  no avaliable mailbox
 * -------------------------------------------------- */
int P2_MboxCreate(int slots, int size)
{
    if(DEBUG_A)
    {
        USLOSS_Console("> P2_MboxCreate(): Started\n");
    }
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    
    
    if(size < 0)
    {
        USLOSS_Console("ERROR: Illegal size!\n");
        return -2;
    }
    if(slots <= 0)
    {
        USLOSS_Console("ERROR: Illegal slots!\n");
        return -2;
    }
    
    P1_P(mailBoxMutex); // Critical section started
    
    for(i = 0; i < P2_MAX_MBOX; i++)
    {
        if(mailBoxList[i].MBoxID == -1)
        {
            
            MBoxFull[i] = P1_SemCreate(0);
            MBoxEmpty[i] = P1_SemCreate(slots);
            
            // ID start from 1
            mailBoxList[i].MBoxID = i + 1;
            mailBoxList[i].slots = slots;
            mailBoxList[i].size = size;
            
            P1_V(mailBoxMutex); // Critical section ended
            
            return mailBoxList[i].MBoxID;
        }
    }
    P1_V(mailBoxMutex); // Critical section ended
    // no avaliable mailbox
    return -3;
}


/* --------------------------------------------------
 *  P2_MboxRelease()
 * --------------------------------------------------
 *  return:  0  released successfully
 *          -1  invalid
 *          -2  illegal input
 * -------------------------------------------------- */
int P2_MboxRelease(int mbox)
{
    if(DEBUG_A)
    {
        USLOSS_Console("> P2_MboxRelease(): Started\n");
    }
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    
    if(mbox < 1 || mbox >= P2_MAX_MBOX)
    {
        USLOSS_Console("ERROR: Mail box ID %d is illegal!\n", mbox);
        return -2;
    }
    
    int index = mbox - 1;
    
    P1_P(mailBoxMutex); // Critical section started
    
    for(i = 0; i < P2_MAX_MBOX; i++)
    {
        if(i == index)
        {
            if(mailBoxList[i].MBoxID != -1)
            {
                if(P1_SemFree(MBoxEmpty[i]) != 0 || P1_SemFree(MBoxFull[i]) != 0)
                {
                    USLOSS_Console("ERROR: Mail box deadlock!\n");
                    USLOSS_Halt(1);
                }
                else
                {
                    while(mailBoxList[i].messageList)
                    {
                        //message * tmp = mailBoxList[i].messageList;
                        mailBoxList[i].messageList = (mailBoxList[i].messageList)->nextMessage;
                    }
                    mailBoxList[i].MBoxID = -1;
                    P1_V(mailBoxMutex); // Critical section ended
                    return 0;
                }
            }
        }
    }
    P1_V(mailBoxMutex); // Critical section ended
    return INVALID;
}


/* --------------------------------------------------
 *  P2_MboxSend()
 * --------------------------------------------------
 *  return:  0  success
 *          -2  illegal input
 * -------------------------------------------------- */
int P2_MboxSend(int mbox, void *msg, int *size)
{
    
    if(DEBUG_A)
    {
        USLOSS_Console("> P2_MboxSend(): Started\n");
    }
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    
    if(mbox < 1 || mbox >= P2_MAX_MBOX)
    {
        USLOSS_Console("ERROR: Mail box ID %d is illegal!\n", mbox);
        return -2;
    }
    
    int index = mbox - 1;
    //USLOSS_Console("<<mbox>> %d\n", mbox);
    if(mailBoxList[index].size < (int) size)
    {
        USLOSS_Console("ERROR: Input size %d is bigger then the max mail box size!\n", (int) size);
        return -2;
    }
    
    if(mailBoxList[index].MBoxID != -1)
    {
        P1_P(MBoxEmpty[index]);
        
        
        mailBoxList[index].slotsUsed++;
        
        // Copy message
        //char * aMessage = malloc(*size);
        void * aMessage = malloc((int) size);
        //strcpy(aMessage, msg);
        memcpy(aMessage, msg, (int) size);
        int num = (int) size;
        insert(&(mailBoxList[index].messageList), aMessage, num);
        //USLOSS_Console(">>>AFTER INS MS: %d\n", (mailBoxList[index].messageList)->messageSize);
        
        P1_V(MBoxFull[index]);
    }
    else    // Not in used
    {
        return INVALID;
    }
    
    return 0;
}


/* --------------------------------------------------
 *  P2_MboxReceive()
 * --------------------------------------------------
 *  return:>=0  message size
 *          -2  illegal input
 * -------------------------------------------------- */
int P2_MboxReceive(int mbox, void *msg, int *size)
{
    
    if(DEBUG_A)
    {
        USLOSS_Console("> P2_MboxReceive(): Started\n");
    }
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    
    if(mbox < 1 || mbox >= P2_MAX_MBOX)
    {
        USLOSS_Console("ERROR: Mail box ID %d is illegal!\n", mbox);
        return -2;
    }
    
    if((int) size < 0)
    {
        USLOSS_Console("ERROR: Input size %d is <0!\n", (int) size);
        return -2;
    }
    
    int index = mbox - 1;
    int messageSize = 0;
    if(mailBoxList[index].MBoxID != -1)
    {
        P1_P(MBoxFull[index]);
        
        
        message * aMessage = mailBoxList[index].messageList;
        mailBoxList[index].messageList = (mailBoxList[index].messageList)->nextMessage;
        
        //USLOSS_Console("<<Msize>>:%d\n", aMessage->messageSize);
        messageSize = aMessage->messageSize;
        
        
        
        if(messageSize > (int) size)
        {
            messageSize = (int) size;
        }
        
        mailBoxList[index].slotsUsed--;
        
        // Copy message
        //char * aMessage = malloc(*size);
        //void * ms = malloc(*size);
        //strcpy(aMessage, msg);
        //memcpy(ms, aMessage, *size);
        //USLOSS_Console("<<size>>:%d, <<Msize>>:%d\n", (int) size, messageSize);
        memcpy(msg, aMessage->message, messageSize);
        
        
        P1_V(MBoxEmpty[index]);
    }
    else
    {
        return INVALID;
    }
    
    return messageSize;
}



/* --------------------------------------------------
 *  P2_MboxCondSend()
 * --------------------------------------------------
 *  return:  0  success
 *          -2  illegal input
 *          -3  full
 * -------------------------------------------------- */
int P2_MboxCondSend(int mbox, void *msg, int *size)
{
    if(DEBUG_A)
    {
        USLOSS_Console("> P2_MboxCondSend(): Started\n");
    }
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    
    if(mbox < 1 || mbox >= P2_MAX_MBOX)
    {
        USLOSS_Console("ERROR: Mail box ID %d is illegal!\n", mbox);
        return -2;
    }
    
    int index = mbox - 1;
    if(mailBoxList[index].size < (int) size)
    {
        USLOSS_Console("ERROR: Input size %d is bigger then the max mail box size!\n", (int) size);
        return -2;
    }
    
    if(mailBoxList[index].MBoxID != -1)
    {
        if(mailBoxList[index].slots > mailBoxList[index].slotsUsed)
        {
            P1_P(MBoxEmpty[index]);
            
            mailBoxList[index].slotsUsed++;
            
            // Copy message
            //char * aMessage = malloc(*size);
            void * aMessage = malloc((int) size);
            //strcpy(aMessage, msg);
            memcpy(aMessage, msg, (int) size);
            
            int num = (int) size;
            insert(&(mailBoxList[index].messageList), aMessage, num);
            //USLOSS_Console(">>>AFTER INS MS: %d\n", (mailBoxList[index].messageList)->messageSize);
            
            P1_V(MBoxFull[index]);
        }
        else
        {
            return -3;
        }
    }
    else    // Not in used
    {
        return INVALID;
    }
    
    
    return 0;
}




/* --------------------------------------------------
 *  P2_MboxCondReceive()
 * --------------------------------------------------
 *  return:>=0  message size
 *          -2  illegal input
 *          -3  no avaliable message
 * -------------------------------------------------- */
int P2_MboxCondReceive(int mbox, void *msg, int *size)
{
    if(DEBUG_A)
    {
        USLOSS_Console("> P2_MboxCondReceive(): Started\n");
    }
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    
    
    if(mbox < 1 || mbox >= P2_MAX_MBOX)
    {
        USLOSS_Console("ERROR: Mail box ID %d is illegal!\n", mbox);
        return -2;
    }
    
    
    if((int) size < 0)
    {
        USLOSS_Console("ERROR: Input size %d is <0!\n", (int) size);
        return -2;
    }
    int index = mbox - 1;
    int messageSize = 0;
    if(mailBoxList[index].MBoxID != -1)
    {
        if(mailBoxList[index].slotsUsed > 0)
        {
            P1_P(MBoxFull[index]);
            
            
            message * aMessage = mailBoxList[index].messageList;
            mailBoxList[index].messageList = (mailBoxList[index].messageList)->nextMessage;
            
            messageSize = aMessage->messageSize;
            
            if(messageSize > (int) size)
            {
                messageSize = (int) size;
            }
            
            mailBoxList[index].slotsUsed--;
            
            // Copy message
            //char * aMessage = malloc(*size);
            //void * ms = malloc(*size);
            //strcpy(aMessage, msg);
            //memcpy(ms, aMessage, *size);
            memcpy(msg, aMessage->message, messageSize);
            
            
            P1_V(MBoxEmpty[index]);
        }
        else
        {
            return -3;
        }
    }
    else
    {
        return INVALID;
    }
    
    return messageSize;
}


/* --------------------------------------------------
 *  P2_Startup()
 * -------------------------------------------------- */
int P2_Startup(void *arg)
{
    P1_Semaphore      running;
    int               status;
    int               pid;
    int               clockPID;
    
    
    if(DEBUG_A)
    {
        USLOSS_Console("> P2_Startup(): Started\n");
    }
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    
    // Set up syscall hander
    if(DEBUG_A_DETAIL)
    {
        USLOSS_Console("* Setting up syscall hander...");
    }
    USLOSS_IntVec[USLOSS_SYSCALL_INT] = syscall_handler;
    if(DEBUG_A_DETAIL)
    {
        USLOSS_Console(" OK\n");
    }
    
    // Initilizing datas
    if(DEBUG_A_DETAIL)
    {
        USLOSS_Console("* Initilizing datas...");
    }
    for(i = 0; i < P1_MAXPROC; i++)
    {
        timeMachine[i].PID = i;
        timeMachine[i].timeStarted = -1;
        timeMachine[i].secondsWait = 0;
        timeMachine[i].timeWaited = 0;
    }
    timerMutex = P1_SemCreate(1);
    
    for(i = 0; i < P2_MAX_MBOX; i++)
    {
        mailBoxList[i].MBoxID = -1;
        mailBoxList[i].slotsUsed = 0;
        mailBoxList[i].messageList = NULL;
    }
    mailBoxMutex = P1_SemCreate(1);
    
    // TODO
    if(DEBUG_A_DETAIL)
    {
        USLOSS_Console(" OK\n");
    }
    
    
    // Create clock device driver
    if(DEBUG_A_DETAIL)
    {
        USLOSS_Console("* Creating clock device driver...");
    }
    running = P1_SemCreate(0);
    clockPID = P1_Fork("Clock driver", ClockDriver, (void *) running, USLOSS_MIN_STACK, 2);
    if(clockPID == -1)
    {
        USLOSS_Console("Can't create clock driver\n");
    }
    else if(DEBUG_A_DETAIL)
    {
        USLOSS_Console(" OK\n");
    }
    
    // Wait for the clock driver to start.
    P1_P(running);
    
    // Create the other device drivers.
    // ...
    
    /*
     * Create initial user-level process. You'll need to check error codes, etc. P2_Spawn
     * and P2_Wait are assumed to be the kernel-level functions that implement the Spawn and
     * Wait system calls, respectively (you can't invoke a system call from the kernel).
     */
    pid = P2_Spawn("P3_Startup", P3_Startup, NULL, 4 * USLOSS_MIN_STACK, 3);
    pid = P2_Wait(&status);
    
    
    // Kill the device drivers
    P1_Kill(clockPID);
    // ...
    
    // Join with the device drivers.
    // ...
    USLOSS_Halt(0);
    return 0;
}


/* --------------------------------------------------
 *  P2_Spawn()
 * --------------------------------------------------
 *  return: -3  invalid priority
 *          -2  stacksize is less than
 *              USLOSS_MIN_STACK (see usloss.h)
 *          -1  no more processes
 *         >=0  PID of created process
 * -------------------------------------------------- */
int P2_Spawn(char *name, int(*func)(void *), void *arg, int stackSize, int priority)
{
    if(DEBUG_A)
    {
        USLOSS_Console("> P2_Spawn(): Spawning %s\n", name);
    }
    
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    
    if(1 > priority || priority > 6)
    {
        return -3;
    }
    /*
     if(stackSize < USLOSS_MIN_STACK)
     {
     return -2;
     }
     */
    // Lots TODOs
    processBlock * aProcess = (processBlock *)malloc(sizeof(processBlock));
    
    strcpy(aProcess->name, name);
    aProcess->priority = priority;
    aProcess->stackSize = stackSize;
    aProcess->startFunc = func;
    aProcess->startArg = arg;
    
    if(DEBUG_A_DETAIL)
    {
        USLOSS_Console("* Forking: %s(%d) with stackSize %d\n", aProcess->name, aProcess->priority, aProcess->stackSize);
    }
    
    int xcode = P1_Fork(aProcess->name, launch, aProcess, aProcess->stackSize, aProcess->priority);
    
    //int xcode = P1_Fork(name, func, arg, stackSize, priority);
    if(DEBUG_A)
    {
        P1_DumpProcesses();
    }
    //USLOSS_Console("XCODE => %d\n", xcode);
    return xcode;
}


/* ------------------------------------------------------------------------
 Name - launch
 Purpose - Dummy function to enable interrupts and launch a given process
 upon startup.
 Returns - nothing
 Side Effects - enable interrupts
 ------------------------------------------------------------------------ */
//void launch(void *arg)
int launch(void *arg)
{
    if(DEBUG_A)
    {
        USLOSS_Console("> launch(): Started\n");
    }
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    
    processBlock * tmp = (processBlock *)arg;
    
    int rc;
    // Enable interrupts
    USLOSS_PsrSet(USLOSS_PsrGet() | USLOSS_PSR_CURRENT_INT);
    
    // Set user mode
    enterUserMode();
    //rc = procTable[pid].startFunc(procTable[pid].startArg);
    rc = tmp->startFunc(tmp->startArg);
    
    /* quit if we ever come back */
    //P1_Quit(rc);
    Sys_Terminate(rc);
    return 0;
}


/* --------------------------------------------------
 *  P2_Terminate()
 * -------------------------------------------------- */
void P2_Terminate(int status)
{
    if(DEBUG_A)
    {
        USLOSS_Console("> P2_Terminate(): Started\n");
    }
    // Check kernel mode
    if(!inKernelMode())
    {
        return;// INVALID;
    }
    
    P1_Quit(status);
    //return status;
}


/* --------------------------------------------------
 *  P2_Wait()
 * --------------------------------------------------
 *  return: -1  the process doesn’t have any children
 *         >=0  the PID of the child that terminated
 * -------------------------------------------------- */
int P2_Wait(int *status)
{
    if(DEBUG_A)
    {
        USLOSS_Console("> P2_Wait(): Started\n");
    }
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    //P1_Quit(status);
    return P1_Join(status);
}


/* --------------------------------------------------
 *  P2_Sleep()
 * --------------------------------------------------
 *  return: 1   seconds is not valid
 *          0   otherwise
 * -------------------------------------------------- */
int P2_Sleep(int seconds)
{
    if(DEBUG_A)
    {
        USLOSS_Console("> P2_Sleep(): Started\n");
    }
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    
    // Wait time invalid
    if(seconds < 0)
    {
        USLOSS_Console("ERROR: Could not sleep for %d (<0) seconds\n", seconds);
        return 1;
    }
    
    P1_Semaphore readyToSleep = P1_SemCreate(0);
    
    P1_P(timerMutex);   // Critical section started
    
    for(i = 0; i < P1_MAXPROC; i++)
    {
        if(timeMachine[i].timeStarted == -1)
        {
            // Do something
            timeMachine[i].timeStarted = USLOSS_Clock();
            timeMachine[i].secondsWait = seconds;
            timeMachine[i].timeWaited = 0;
            timerSemaphore[i] = readyToSleep;
        }
        if(DEBUG_A_DETAIL && 0)
        {
            printTimers();
        }
    }
    P1_V(timerMutex);   // Critical section ended
    
    P1_P(readyToSleep);
    P1_SemFree(readyToSleep);   // Free semaphore
    return 0;
}


/* --------------------------------------------------
 *  ClockDriver()
 * -------------------------------------------------- */
static int ClockDriver(void *arg)
{
    P1_Semaphore running = (P1_Semaphore) arg;
    int result;
    int status;
    int rc = 0;
    
    /*
     * Let the parent know we are running and enable interrupts.
     */
    P1_V(running);
    USLOSS_PsrSet(USLOSS_PsrGet() | USLOSS_PSR_CURRENT_INT);
    while(1)
    {
        result = P1_WaitDevice(USLOSS_CLOCK_DEV, 0, &status);
        if (result != 0)
        {
            rc = 1;
            goto done;
        }
        
        /*
         * Compute the current time and wake up any processes
         * whose time has come.
         */
        P1_P(timerMutex);   // Critical section started
        
        for(i = 0; i < P1_MAXPROC; i++)
        {
            int waitTime = timeMachine[i].secondsWait * 1000000.0;
            timeMachine[i].timeWaited = USLOSS_Clock() - timeMachine[i].timeStarted;
            
            if(timeMachine[i].timeStarted != -1 && waitTime <= timeMachine[i].timeWaited)
            {
                P1_V(timerSemaphore[i]);
                timeMachine[i].timeStarted = -1;
                timeMachine[i].secondsWait = 0;
                timeMachine[i].timeWaited = 0;
            }
            if(DEBUG_A_DETAIL && 0)
            {
                printTimers();
            }
        }
        P1_V(timerMutex);   // Critical section ended
        
    }
done:
    return rc;
}


static void printTimers()
{
    USLOSS_Console("Timer[%d]: Started at %d, need to wait for %ds, has already waited for %d\n", timeMachine[i].PID, timeMachine[i].timeStarted, timeMachine[i].secondsWait, timeMachine[i].timeWaited);
}

/* --------------------------------------------------
 *  syscall_handler()
 * -------------------------------------------------- */
static void syscall_handler(int type, void *offset)
{
    if(DEBUG_A_DETAIL)
    {
        USLOSS_Console("> syscall_handler(): Started\n");
    }
    if(offset == NULL)
    {
        USLOSS_Console("ERROR: USLOSS_Sysargs can NOT be NULL!\n");
        return;
    }
    USLOSS_Sysargs *sysargs = (USLOSS_Sysargs *) offset;
    int syscallNum = sysargs->number;
    
    USLOSS_PsrSet(USLOSS_PsrGet() | USLOSS_PSR_CURRENT_INT);
    
    switch(syscallNum)
    {
        case SYS_TERMREAD:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_TERMREAD called\n");
            }
            //sysargs->arg4 = (void *) Sys_TermRead((char *)sysargs->arg1, (int)sysargs->arg2, (int)sysargs->arg3, (int *)sysargs->arg2);
            break;
            
            
        case SYS_TERMWRITE:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_TERMWRITE called\n");
            }
            //sysargs->arg4 = (void *) Sys_TermWrite((char *)sysargs->arg1, (int)sysargs->arg2, (int)sysargs->arg3, (int *)sysargs->arg2);
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_SPAWN:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_SPAWN called\n");
            }
            
            /*
             Input:
             arg1: address of the function to spawn
             arg2: parameter passed to spawned function
             arg3: stack size (in bytes)
             arg4: priority
             arg5: character string containing process’s name
             
             Output:
             arg1: the PID of the newly created process; -1 if a process could not be created
             arg4: -1 if illegal values are given as input; 0 otherwise.
             */
            // PID of the newly created process
            sysargs->arg1 = (void *) P2_Spawn((char *) sysargs->arg5, sysargs->arg1, sysargs->arg2, (int) sysargs->arg3, (int) sysargs->arg4);
            // int P2_Spawn(char *name, int(*func)(void *), void *arg, int stackSize, int priority)
            
            if((int) sysargs->arg1 > 0)
            {
                sysargs->arg4 = (void *) 0;
            }
            else
            {
                sysargs->arg1 = (void *) INVALID;
                sysargs->arg4 = (void *) INVALID;
            }
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_WAIT:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_WAIT called\n");
            }
            
            /*
             Output:
             arg1: process id of the terminating child.
             arg2: the termination code of the child
             arg4: -1 if the process has no children, 0 otherwise
             */
            int tcode;
            sysargs->arg1 = (void *) P2_Wait(&tcode);
            if((int) sysargs->arg1 > 0)
            {
                sysargs->arg2 = (void *) tcode;
                sysargs->arg4 = (void *) 0;
            }
            else
            {
                sysargs->arg1 = (void *) INVALID;
                sysargs->arg2 = (void *) INVALID;
                sysargs->arg4 = (void *) INVALID;
            }
            
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_TERMINATE:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_TERMINATE called\n");
            }
            
            /*
             Input:
             arg1: termination code for the process (used as status parameter to P1_Quit).
             */
            P2_Terminate((int) sysargs->arg1);
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_MBOXCREATE:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_MBOXCREATE called\n");
            }
            /*
             Input:
             arg1: number of messages the mailbox can hold (must be > 0)
             arg2: maximum size of a message (in bytes, must be >= 0)
             
             Output:
             arg1: mailbox ID, -1 if there are no more mailboxes
             arg4: -1 if illegal values are given as input; 0 otherwise.
             */
            int xcodeC = P2_MboxCreate((int) sysargs->arg1, (int) sysargs->arg2);
            if(xcodeC == -3)
            {
                sysargs->arg1 = (void *) -1;
                sysargs->arg4 = (void *) 0;
            }
            else if(xcodeC == -2)
            {
                sysargs->arg1 = (void *) -1;
                sysargs->arg4 = (void *) -1;
            }
            else
            {
                sysargs->arg1 = (void *) xcodeC;
                sysargs->arg4 = (void *) 0;
            }
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_MBOXRELEASE:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_MBOXRELEASE called\n");
            }
            /*
             Input:
             arg1: ID of mailbox to release
             
             Output:
             arg4: -1 if illegal values are given as input; 0 otherwise.
             If a mailbox is released on which processes are waiting then an error message is printed and USLOSS_Halt(1) is called.
             */
            int xcodeR = P2_MboxRelease((int) sysargs->arg1);
            if(xcodeR != 0)
            {
                sysargs->arg4 = (void *) -1;
            }
            else
            {
                sysargs->arg4 = (void *) 0;
            }
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_MBOXSEND:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_MBOXSEND called\n");
            }
            /*
             Input:
             arg1: the ID of the mailbox to which to send
             arg2: pointer to the message to be sent
             arg3: size of the message (must be <= mailbox's maximum message size)
             
             Output:
             arg4: -1 if illegal values are given as input; 0 otherwise.
             */
            int xcodeS = P2_MboxSend((int) sysargs->arg1, sysargs->arg2, (int *) sysargs->arg3);
            if(xcodeS != 0)
            {
                sysargs->arg4 = (void *) -1;
            }
            else
            {
                sysargs->arg4 = (void *) 0;
            }
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_MBOXRECEIVE:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_MBOXRECEIVE called\n");
            }
            /*
             Input:
             arg1: the ID of the mailbox from which to receive
             arg2: pointer to a buffer in which to place the received message
             arg3: size of the buffer
             
             Output:
             arg2: size of the message received
             arg4: -1 if illegal values are given as input; 0 otherwise.
             */
            int xcodeRR = P2_MboxReceive((int) sysargs->arg1, sysargs->arg2, (int *) sysargs->arg3);
            if(xcodeRR >= 0)//> 0) // Size can be 0?
            {
                sysargs->arg2 = (void *) xcodeRR;
                sysargs->arg4 = (void *) 0;
            }
            else
            {
                //sysargs->arg2 = (void *) xcodeRR;
                sysargs->arg4 = (void *) -1;
            }
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_MBOXCONDSEND:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_MBOXCONDSEND called\n");
            }
            /*
             Input:
             arg1: the ID of the mailbox to which to send
             arg2: pointer to the message to be sent
             arg3: size of the message (must be <= mailbox's maximum message size)
             
             Output:
             arg4: -1 if illegal values are given as input; 0 if the operation succeeded; 1 if the mailbox was full.
             */
            int xcodeCS = P2_MboxCondSend((int) sysargs->arg1, sysargs->arg2, (int *) sysargs->arg3);
            if(xcodeCS == 0)
            {
                sysargs->arg4 = (void *) 0;
            }
            else if(xcodeCS == -3)
            {
                sysargs->arg4 = (void *) 1;
            }
            else
            {
                sysargs->arg4 = (void *) -1;
            }
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_MBOXCONDRECEIVE:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_MBOXCONDRECEIVE called\n");
            }
            /*
             Input:
             arg1: the ID of the mailbox from which to receive
             arg2: pointer to a buffer in which to place the received value.
             arg3: size of the buffer
             
             Output:
             arg2: size of the message received
             arg4: -1 if illegal values are given as input; 0 if the operation succeeded; 1 if no message was available.
             */
            int xcodeCR = P2_MboxCondReceive((int) sysargs->arg1, sysargs->arg2, (int *)sysargs->arg3);
            if(xcodeCR >= 0)
            {
                sysargs->arg2 = (void *) xcodeCR;
                sysargs->arg4 = (void *) 0;
            }
            else if(xcodeCR == -3)
            {
                //sysargs->arg2 = (void *) xcodeCR;
                sysargs->arg4 = (void *) 1;
            }
            else if(xcodeCR == -2)
            {
                //sysargs->arg2 = (void *) xcodeCR;
                sysargs->arg4 = (void *) -1;
            }
            else
            {
                USLOSS_Console("ERROR: not in kernel mode or other problem.\n");
                sysargs->arg4 = (void *) -1;
            }
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_SLEEP:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_SLEEP called\n");
            }
            
            /*
             Input:
             arg1: number of seconds to delay the process
             
             Output:
             arg4: -1 if illegal values are given as input; 0 otherwise.
             */
            sysargs->arg4 = (void *) P2_Sleep((int) sysargs->arg1);
            break;
            
            
        case SYS_DISKREAD:
            break;
            
            
        case SYS_DISKWRITE:
            break;
            
            
        case SYS_DISKSIZE:
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_SEMCREATE:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_SEMCREATE called\n");
            }
            /*
             Input:
             arg1: initial semaphore value
             
             Output:
             arg1: semaphore handle to be used in subsequent semaphore system calls
             arg4: -1 if initial value is negative, 0 otherwise
             */
            if((int) sysargs->arg1 < 0)
            {
                sysargs->arg4 = (void *) -1;
            }
            else
            {
                sysargs->arg1 = (void *) P1_SemCreate((int) sysargs->arg1);
                sysargs->arg4 = (void *) 0;
            }
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_SEMP:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_SEMP called\n");
            }
            /*
             Input:
             arg1: semaphore handle
             
             Output:
             arg4: -1 if semaphore handle is invalid, 0 otherwise
             */
            P1_Semaphore semP = (P1_Semaphore) sysargs->arg1;
            if(P1_P(semP) != INVALID)
            {
                sysargs->arg4 = (void *) 0;
            }
            else
            {
                sysargs->arg4 = (void *) -1;
            }
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_SEMV:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_SEMV called\n");
            }
            /*
             Input:
             arg1: semaphore handle
             
             Output:
             arg4: -1 if semaphore handle is invalid, 0 otherwise
             */
            P1_Semaphore semV = (P1_Semaphore) sysargs->arg1;
            if(P1_V(semV) != INVALID)
            {
                sysargs->arg4 = (void *) 0;
            }
            else
            {
                sysargs->arg4 = (void *) -1;
            }
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_SEMFREE:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_SEMFREE called\n");
            }
            /*
             Input:
             arg1: semaphore handle
             
             Output:
             arg4: -1 if semaphore handle is invalid, 0 otherwise
             */
            P1_Semaphore semFree = (P1_Semaphore) sysargs->arg1;
            if(P1_SemFree(semFree) != INVALID)
            {
                sysargs->arg4 = (void *) 0;
            }
            else
            {
                sysargs->arg4 = (void *) -1;
            }
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_GETTIMEOFDAY:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_GETTIMEOFDAY called\n");
                USLOSS_Console("* Time of day: %d\n", USLOSS_Clock());
            }
            
            /*
             Output:
             arg1: the time of day
             */
            sysargs->arg1 = (void *) USLOSS_Clock();
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_CPUTIME:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_CPUTIME called\n");
                USLOSS_Console("* CPU Time: %d\n", P1_ReadTime());
            }
            
            /*
             Output:
             arg1: the process’s CPU time
             */
            sysargs->arg1 = (void *) P1_ReadTime();
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_GETPID:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_GETPID called\n");
                USLOSS_Console("* Current PID: %d\n", P1_GetPID());
            }
            
            /*
             Output:
             arg1: the process’s PID
             */
            sysargs->arg1 = (void *) P1_GetPID();
            break;
            
            
            /* DONE */
            /* PART A */
        case SYS_DUMPPROCESSES:
            if(DEBUG_A_DETAIL)
            {
                USLOSS_Console("* SYS_DUMPPROCESSES called\n");
            }
            
            /*
             Print process information to the console (P1_DumpProcesses).
             */
            P1_DumpProcesses();
            break;
            
            
        default:
            USLOSS_Console("ERROR: Unknown syscall!\n");
            break;
    }
}


/* --------------------------------------------------
 *  enterUserMode()
 * -------------------------------------------------- */
static void enterUserMode()
{
    USLOSS_PsrSet((USLOSS_PsrGet() >> 1) << 1);
}


/* --------------------------------------------------
 *  inKernelMode()
 * --------------------------------------------------
 *  The functions provided by the kernel may only be
 *  called by processes running in kernel mode. The
 *  kernel should confirm this and in case of
 *  execution by a user mode process, should print an
 *  error message and return without executing the
 *  function.
 *
 *  return: 1   it's in user mode
 0   not in user mode
 * -------------------------------------------------- */
static int inKernelMode()
{
    if(DEBUG_A)
    {
        USLOSS_Console("* Checking kernel mode... ");
    }
    if((USLOSS_PsrGet() & 0x1) != 1)
    {
        
        USLOSS_Console("ERROR: NOT in kernel mode!\n");
        return 0;
    }
    if(DEBUG_A)
    {
        USLOSS_Console("OK\n");
    }
    return 1;
}


/* --------------------------------------------------
 *  linked list operations
 * -------------------------------------------------- */
static void insert(message **list, void * msg, int size)
{
    if(*list == NULL)
    {
        *list = (message *)malloc(sizeof(message));
        (*list)->message = msg;
        (*list)->messageSize = size;
        (*list)->nextMessage = NULL;
    }
    else
    {
        message * tmp = *list;
        while(tmp->nextMessage)
        {
            tmp = tmp->nextMessage;
        }
        
        message * aMessage = (message *)malloc(sizeof(message));
        aMessage->message = msg;
        aMessage->messageSize = size;
        aMessage->nextMessage = NULL;
        
        tmp->nextMessage = aMessage;
    }
    
    //USLOSS_Console("<SIZE>%d\n", size);
}
