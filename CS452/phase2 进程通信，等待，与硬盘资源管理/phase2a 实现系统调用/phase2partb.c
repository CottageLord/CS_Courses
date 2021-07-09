/**
 * phase2.c
 * Part B
 *
 * @author: David Carrig
 * @author: Hercy (Yunhao Zhang)
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
#define DEBUG_B         1
#define DEBUG_B_DETAIL  1

#define INVALID        -1

static int i;
static int finished;

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
static int TerminalReader(void *arg);

P1_Semaphore running;


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
 *  Terminal Driver Stuff
 * -------------------------------------------------- */
#define MAX_TERMINAL USLOSS_TERM_UNITS   //USLOSS_TERM_INT

#define READ        0
#define WRITE       1
#define NEXT        2
#define MESSAGE     3
#define WAIT        4

static int TerminalDriver(void *arg);

typedef struct terminal
{
    int PID;
    int MBox[4];

    P1_Semaphore active;
    P1_Semaphore mutex;
    
} terminal;

terminal terminals[MAX_TERMINAL];


/* --------------------------------------------------
 *  Disk Driver Stuff
 * -------------------------------------------------- */
#define MAX_DISK USLOSS_DISK_UNITS //USLOSS_DISK_INT

static int DiskDriver(void *arg);

typedef struct disk
{
    int PID;
    P1_Semaphore mutex;
} disk;

disk disks[MAX_DISK];


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

P1_Semaphore mailBoxMutexList[P2_MAX_MBOX]; // Handle critical sections



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
            
            mailBoxMutexList[i] = P1_SemCreate(1);
            
            // ID start from 1
            mailBoxList[i].MBoxID = i + 1;
            mailBoxList[i].slots = slots;
            mailBoxList[i].size = size;
            
            P1_V(mailBoxMutex); // Critical section ended
            
            return mailBoxList[i].MBoxID;
        }
    }
    // no avaliable mailbox
    P1_V(mailBoxMutex); // Critical section ended
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
    if(DEBUG_A_DETAIL)
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
                if(P1_SemFree(MBoxEmpty[i]) != 0 || P1_SemFree(MBoxFull[i]) != 0 || P1_SemFree(mailBoxMutexList[i]) != 0)
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
    
    if(DEBUG_A_DETAIL)
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
        
        P1_P(mailBoxMutexList[index]);  // Critical section started
        
        mailBoxList[index].slotsUsed++;
        
        // Copy message
        //char * aMessage = malloc(*size);
        void * aMessage = malloc((int) size);
        //strcpy(aMessage, msg);
        memcpy(aMessage, msg, (int) size);
        int num = (int) size;
        insert(&(mailBoxList[index].messageList), aMessage, num);
        //USLOSS_Console(">>>AFTER INS MS: %d\n", (mailBoxList[index].messageList)->messageSize);
        
        
        P1_V(mailBoxMutexList[index]);  // Critical section ended
        
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
    
    if(DEBUG_A_DETAIL)
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
        
        
        P1_P(mailBoxMutexList[index]);  // Critical section started
        
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
        
        P1_V(mailBoxMutexList[index]);  // Critical section ended
        
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
    if(DEBUG_A_DETAIL)
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
        P1_P(mailBoxMutexList[index]);  // Critical section started
        
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
            
            P1_V(mailBoxMutexList[index]);  // Critical section ended
            
            P1_V(MBoxFull[index]);
            
            
        }
        else
        {
            P1_V(mailBoxMutexList[index]);  // Critical section ended
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
    if(DEBUG_A_DETAIL)
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
        P1_P(mailBoxMutexList[index]);  // Critical section started
        
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
            
            P1_V(mailBoxMutexList[index]);  // Critical section ended
            
            P1_V(MBoxEmpty[index]);
        }
        else
        {
            P1_V(mailBoxMutexList[index]);  // Critical section ended
            
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
    //P1_Semaphore      running;
    int               status;
    int               pid;
    int               clockPID;
    finished = 0;
    
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
    running = P1_SemCreate(0);
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
    
    // Initializing terminals
    /*for(i = 0; i < MAX_TERMINAL; i++)
    {
        terminals[i].PID = -1;
    }
    
    // Initializing disks
    for(i = 0; i < MAX_DISK; i++)
    {
        disks[i].PID = -1;
        disks[i].mutex = P1_SemCreate(1);
    }
    */
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
    
    
    // Create disk device driver
    for(i = 0; i < MAX_DISK; i++)
    {
        if(DEBUG_B_DETAIL)
        {
            USLOSS_Console("* Creating disk device driver(%d)...", i);
        }
        
        disks[i].PID = -1;
        disks[i].mutex = P1_SemCreate(1);
        
        int aNewPID = P1_Fork("Disk driver", DiskDriver, (void *) i,  USLOSS_MIN_STACK, 2);//(void *) running, USLOSS_MIN_STACK, 2);
        
        // Disk device is invalid
        if(aNewPID == INVALID)
        {
            USLOSS_Console("ERROR: Unable to create disk driver(%d)\n", i);
        }
        // Created
        else
        {
            disks[i].PID = aNewPID;
            
            if(DEBUG_B_DETAIL)
            {
                USLOSS_Console(" OK\n");
            }
            
        }
        
        // Wait for the disk driver to start.
        P1_P(running);
    }
    

    
    // Create terminal device driver
    for(i = 0; i < MAX_TERMINAL; i++)
    {
        if(DEBUG_B_DETAIL)
        {
            USLOSS_Console("* Creating disk terminal driver(%d)...", i);
        }
        terminals[i].PID = -1;
        
        terminals[i].mutex = P1_SemCreate(1);
        terminals[i].active = P1_SemCreate(0);
        
        terminals[i].MBox[WRITE] = P2_MboxCreate(10, P2_MAX_LINE);
        terminals[i].MBox[READ] = P2_MboxCreate(1, sizeof(int));
        terminals[i].MBox[NEXT] = P2_MboxCreate(1, sizeof(int));
        
        int aNewPID = P1_Fork("Terminal driver", TerminalDriver, (void *) i,  USLOSS_MIN_STACK, 2);//(void *) running, USLOSS_MIN_STACK, 2);
        
        // Terminal device is invalid
        if(aNewPID == INVALID)
        {
            USLOSS_Console("ERROR: Unable to create terminal driver(%d)\n", i);
        }
        // Created
        else
        {
            terminals[i].PID = aNewPID;
            
            if(DEBUG_B_DETAIL)
            {
                USLOSS_Console(" OK\n");
            }
            
        }
        
        // Wait for the terminal driver to start.
        //P1_P(running); // Not ready yet
    }

    
    /*
     * Create initial user-level process. You'll need to check error codes, etc. P2_Spawn
     * and P2_Wait are assumed to be the kernel-level functions that implement the Spawn and
     * Wait system calls, respectively (you can't invoke a system call from the kernel).
     */
    pid = P2_Spawn("P3_Startup", P3_Startup, NULL, 4 * USLOSS_MIN_STACK, 1);
    pid = P2_Wait(&status);
    
    
    // Kill the device drivers
    P1_Kill(clockPID);
    finished = 1;
    
    
    
    for(i = 0; i < MAX_TERMINAL; i++)
    {
        P1_Kill(terminals[i].PID);
    }
    
    for(i = 0; i < MAX_DISK; i++)
    {
        P1_Kill(disks[i].PID);
    }
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
 *  return: -1  the process doesnít have any children
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
    //P1_Semaphore
    running = (P1_Semaphore) arg;
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











/* TODO */
/* --------------------------------------------------
 *  TerminalDriver()
 * -------------------------------------------------- */
static int TerminalDriver(void *arg)
{
    if(DEBUG_B)
    {
        USLOSS_Console("> TerminalDriver(): Started\n");
    }
    
    int s = 0;
    int s0 = 0;
    int c0 = 0;
    int c1 = 0;
    int c2 = 0;
    
    
    P1_Fork("Terminal Reader", TerminalReader, arg, USLOSS_MIN_STACK, 2);
    
    
    
    P1_P(terminals[(int)arg].active);
    
    
    int xcodeDO = USLOSS_DeviceOutput(USLOSS_TERM_DEV, (int)arg, (void *) USLOSS_TERM_CTRL_RECV_INT(c1));
    
    if(DO != USLOSS_DEV_OK)
    {
        return INVALID;
    }
    
    
    P1_V(running);
    
    while(finished == 0)
    {
        
        if(P1_WaitDevice(USLOSS_TERM_DEV, (int)arg, &s))
        {
            break;
        }
        else
        {
        
            
            /*
             * These are the fields of the terminal status registers. A call to
             * USLOSS_DeviceInput will return the status register, and you can use these
             * macros to extract the fields. The xmit and recv fields contain the
             * status codes listed above.
             
             #define USLOSS_TERM_STAT_CHAR(status) (((status) >> 8) & 0xff)
             character received, if any
            
             #define USLOSS_TERM_STAT_XMIT(status) (((status) >> 2) & 0x3)
             xmit status for unit
            
             #define	USLOSS_TERM_STAT_RECV(status) ((status) & 0x3)
             recv status for unit
             */
            s0 = USLOSS_TERM_STAT_RECV(s);
            s = 0;
            int sizeOfInt = sizeof(int);
            
            if(s0 == USLOSS_DEV_BUSY)
            {
                
                
                int status = P2_MboxSend(terminals[(int)arg].MBox[READ], &s, &sizeOfInt);
                
                if(status == -2)
                {
                    USLOSS_Console("ERROR: Illegal input.\n");
                }
            }
            
            // xmit status for unit
            s0 = USLOSS_TERM_STAT_XMIT(s);
            if(s0 == USLOSS_DEV_OK)
            {
                
                /*
                 * These are the fields of the terminal control registers. You can use
                 * these macros to put together a control word to write to the
                 * control registers via USLOSS_DeviceOutput.
                 
                 #define USLOSS_TERM_CTRL_CHAR(ctrl, ch) ((ctrl) | (((ch) & 0xff) << 8))
                 char to send, if any
                 
                 #define	USLOSS_TERM_CTRL_XMIT_INT(ctrl) ((ctrl) | 0x4)
                 enable xmit interrupts
                 
                 #define	USLOSS_TERM_CTRL_RECV_INT(ctrl) ((ctrl) | 0x2)
                 enable recv interrupts
                 
                 #define USLOSS_TERM_CTRL_XMIT_CHAR(ctrl) ((ctrl) | 0x1)
                 xmit the char in the upper bits
                 */
                int status = P2_MboxCondReceive(terminals[(int)arg].MBox[WRITE], &c0, &sizeOfInt);
                
                // No messages
                if(status == -3)
                {
                    
                    // char to send, if any
                    //c2 = USLOSS_TERM_CTRL_CHAR(c2, c0);
                    
                    // xmit the char in the upper bits
                    //c2 = USLOSS_TERM_CTRL_XMIT_CHAR(c2);
                    
                    // enable recv interrupts
                    //c2 = USLOSS_TERM_CTRL_RECV_INT(c2);
                    //c2 = USLOSS_TERM_CTRL_CHAR(c2, c0);
                    
                    // enable recv interrupts
                    c2 = USLOSS_TERM_CTRL_RECV_INT(0);
                    
                    USLOSS_DeviceOutput(USLOSS_TERM_DEV, (int)arg, (void *)c2);
                }
                else
                {

                    // char to send, if any
                    c2 = USLOSS_TERM_CTRL_CHAR(0, c0);
                    
                    // xmit the char in the upper bits
                    c2 = USLOSS_TERM_CTRL_XMIT_CHAR(c2, 0);
                    
                    // enable recv interrupts
                    c2 = USLOSS_TERM_CTRL_RECV_INT(c2);
                    //c2 = USLOSS_TERM_CTRL_CHAR(c2, c0);
                    
                    // enable xmit interrupts
                    c2 = USLOSS_TERM_CTRL_XMIT_INT(c2);
                    
                    
                    
                    
                    USLOSS_DeviceOutput(USLOSS_TERM_DEV, (int)arg, (void *)c2);
                }
            }
        }
    }
    
    c1 = USLOSS_TERM_CTRL_RECV_INT(c1);
    int x = P2_MboxCondSend(terminals[(int)arg].MBox[READ], &c1, &s);
    
    switch(x)
    {
        case -3:
            USLOSS_Console("Fulled.\n");
            break;
            
        case -2:
            USLOSS_Console("Invalid input.\n");
            break;
    }
    
    
    P1_Join(&s);
    
    return (int)arg;
}


int TerminalReader(void *arg)
{
    int msg = 0;
    char mbuff[P2_MAX_LINE];
    char newLine = '\n';
    char curr;
    
    //terminal t = terminals[(int)arg];
    
    P1_V(terminals[(int)arg].active);
    
    for(i = 0; finished < 1; i++)
    {
        int sizeOfInt = sizeof(int);
        
        int xcode = P2_MboxReceive(terminals[(int)arg].MBox[READ], &msg, &sizeOfInt);
        
        if(xcode == -2)
        {
            USLOSS_Console("ERROR: Illegal input.\n");
            
            return INVALID;
        }
        
        curr = USLOSS_TERM_STAT_CHAR(msg);
        
        if(i < P2_MAX_LINE)
        {
            mbuff[i] = curr;
        }
        else if(P2_MAX_LINE < i && curr == newLine)
        {
            i = P2_MAX_LINE;
        }
        
        if(curr == newLine)
        {
            
            P2_MboxCondSend(terminals[(int)arg].MBox[NEXT], mbuff, &i);
            
            memset(mbuff, 0, P2_MAX_LINE);
            
            
            i = 0;
            i--;
        }
    }
    return (int)arg;
}



/* TODO */
/* --------------------------------------------------
 *  P2_TermRead()
 * --------------------------------------------------
 *  return:
 *      -1:     invalid parameters
 *      >0:     number of characters read
 * -------------------------------------------------- */
int P2_TermRead(int unit, int size, char *buffer)
{
    
    if(DEBUG_B)
    {
        USLOSS_Console("> P2_TermRead(): Started\n");
    }
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    
    // Check input
    if(size < 0)
    {
        return INVALID;
    }
    int index = size - 1;
    
    if(!(unit >= 0 && unit < MAX_TERMINAL))
    {
        return INVALID;
    }
    
    //P1_P(terminals[unit].mutex); // Critical section started
    
    terminal t = terminals[unit];
    int mailBoxID = t.MBox[NEXT];
    
    
    
    int xcode = P2_MboxReceive(mailBoxID, buffer, &size);
    
    //P1_V(terminals[unit].mutex); // Critical section ended
    if(size != xcode)
    {
        buffer[xcode] = '\0';
        xcode++;
    }
    else
    {
        if(index < 0)
        {
            USLOSS_Console("ERROR: Index(%d) is < 0\n", index);
            return INVALID;
        }
        else
        {
            buffer[index] = '\0';
        }
    }
    return xcode;
}



/* TODO */
/* --------------------------------------------------
 *  P2_TermWrite()
 * --------------------------------------------------
 *  return:
 *      -1:     invalid parameters
 *      >0:     number of characters written
 * -------------------------------------------------- */
int P2_TermWrite(int unit, int size, char *text)
{
    if(DEBUG_B)
    {
        USLOSS_Console("> P2_TermWrite(): Started\n");
    }
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    
    // Check input
    if(size < 0)
    {
        return INVALID;
    }
    if(!(unit >= 0 && unit < MAX_TERMINAL))
    {
        return INVALID;
    }
    
    int c0 = 0;
    int c1 = 0;
    
    //terminal t = terminals[unit];
    if(size >= P2_MAX_LINE)
    {
        size = P2_MAX_LINE;
    }
    
    P1_P(terminals[unit].mutex); // Critical section started
    
    for(i = 0; i < size; i++)
    {
        /*
         * These are the fields of the terminal control registers. You can use
         * these macros to put together a control word to write to the
         * control registers via USLOSS_DeviceOutput.
         
         #define USLOSS_TERM_CTRL_CHAR(ctrl, ch) ((ctrl) | (((ch) & 0xff) << 8))
         char to send, if any
        
         #define	USLOSS_TERM_CTRL_XMIT_INT(ctrl) ((ctrl) | 0x4)
         enable xmit interrupts
        
         #define	USLOSS_TERM_CTRL_RECV_INT(ctrl) ((ctrl) | 0x2)
         enable recv interrupts
        
         #define USLOSS_TERM_CTRL_XMIT_CHAR(ctrl) ((ctrl) | 0x1)
         xmit the char in the upper bits */
        c0 = 0;
        c0 = USLOSS_TERM_CTRL_RECV_INT(c0);
        //USLOSS_TERM_CTRL_XMIT_INT
        // enable xmit interrupts
        c0 = USLOSS_TERM_CTRL_XMIT_INT(c0);
        
        USLOSS_DeviceOutput(USLOSS_TERM_DEV, unit, (void *) c0);
        
        c1 = *(text + i);
        
        //P2_MboxCondSend(t.MBox[WRITE], &c1, &(sizeof(char)));
        int x = P2_MboxSend(terminals[unit].MBox[WRITE], &c1, (int *)sizeof(char));
        
        if(x == -2)
        {
            USLOSS_Console("ERROR: Invalid input\n");
            return INVALID;
        }
    }
    
    
    P1_V(terminals[unit].mutex);  // Critical section started
    
    return size;
}





/* TODO */
/* --------------------------------------------------
 *  DiskDriver()
 * -------------------------------------------------- */
static int DiskDriver(void *arg)
{
    if(DEBUG_A)
    {
        USLOSS_Console("> DiskDriver(): Started\n");
    }
    
    //P1_Semaphore running = (P1_Semaphore) arg;
    
    int status;
    //int rc = 0;
    
    /*
     * Let the parent know we are running and enable interrupts.
     */
    P1_V(running);
    USLOSS_PsrSet(USLOSS_PsrGet() | USLOSS_PSR_CURRENT_INT);
    
    /*
     int P1_WaitDevice(int type, int unit, int *status)
     -3:		the process was killed
     -2: 	invalid type
     -1:		invalid unit
     0:		success
     */
    while(P1_WaitDevice(USLOSS_CLOCK_DEV, (int) arg, &status) == 0)
    {
        /*
        switch(status)
        {
            case USLOSS_DEV_READY:
                if(DEBUG_B)
                {
                    USLOSS_Console("* Status returned with USLOSS_DEV_READY\n");
                }
                break;
                
                
            case USLOSS_DEV_BUSY:
                if(DEBUG_B)
                {
                    USLOSS_Console("* Status returned with USLOSS_DEV_BUSY\n");
                }
                break;
                
                
            case USLOSS_DEV_ERROR:
                USLOSS_Console("ERROR: Disk driver error\n");
                break;
                
            default:
                USLOSS_Console("ERROR: Werid status code %d\n", status);
                break;
        }*/
    }
    
    // Status result for last row
    switch(status)
    {
        case USLOSS_DEV_READY:
            if(DEBUG_B)
            {
                USLOSS_Console("* Status returned with USLOSS_DEV_READY\n");
            }
            break;
            
            
        case USLOSS_DEV_BUSY:
            if(DEBUG_B)
            {
                USLOSS_Console("* Status returned with USLOSS_DEV_BUSY\n");
            }
            break;
            
            
        case USLOSS_DEV_ERROR:
            USLOSS_Console("ERROR: Disk driver error\n");
            break;
            
        default:
            USLOSS_Console("ERROR: Werid status code %d\n", status);
            break;
    }
    
    return 1;
    //done:
    //return rc;
}

/* TODO */
/* --------------------------------------------------
 *  P2_DiskRead()
 * --------------------------------------------------
 *  return:
 *      -1:     invalid parameters
 *       0:     sectors were read successfully
 *      >0:     diskís status register
 * -------------------------------------------------- */
int	P2_DiskRead(int unit, int track, int first, int sectors, void *buffer)
{
    if(DEBUG_B)
    {
        USLOSS_Console("> P2_DiskRead(): Started\n");
    }
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    
    // Check inputs
    if(!(unit >= 0 && unit < MAX_DISK)) // 0 ~ MAX_DISK
    {
        return INVALID;
    }
    if(!(track >= 0 && first >= 0))
    {
        return INVALID;
    }
    if(!(sectors > 0 && sectors < USLOSS_DISK_TRACK_SIZE))
    {
        return INVALID;
    }
    
    
    // TODO
    P1_P(disks[unit].mutex);    // Critical section started
    /*
     typedef struct USLOSS_DeviceRequest
     {
     int opr;
     void *reg1;
     void *reg2;
     } USLOSS_DeviceRequest;
     */
    
    
    /*
     opr is USLOSS_DISK_READ or USLOSS_DISK_WRITE:
     
     reg1: the index of the sector to be read or
     written within the current track
     
     reg2: a pointer to a 512-byte buffer into
     which data from the disk will be read or from
     which data will be written.
     */
    // Read from the given track
    USLOSS_DeviceRequest * read = malloc(sizeof(USLOSS_DeviceRequest));
    read->reg1 = (void *) first;
    read->reg2 = buffer;    // 512-byte buffer
    read->opr = USLOSS_DISK_READ;
    
    /*
     opr is USLOSS_DISK_SEEK:
     
     reg1: the track number to which the diskís
     read/write head should be moved.
     */
    // Seeking to a given track
    USLOSS_DeviceRequest * seek = malloc(sizeof(USLOSS_DeviceRequest));
    seek->reg1 = (void *) track;
    //seek->reg2 = buffer;
    seek->opr = USLOSS_DISK_SEEK;
    
    
    // Execute seek request
    USLOSS_DeviceOutput(USLOSS_DISK_DEV, unit, seek);
    
    int status = 0;
    //USLOSS_DeviceInput(USLOSS_DISK_DEV, unit, &status);
    P1_WaitDevice(USLOSS_DISK_DEV, unit, &status);
    
    
    while(status == USLOSS_DEV_READY)
    {
        // Run out of sectors and tracks
        if(sectors == 0 || first >= USLOSS_DISK_TRACK_SIZE)
        {
            P1_V(disks[unit].mutex);    // Critical section ended
            return status;
        }
        
        // Execute read request
        USLOSS_DeviceOutput(USLOSS_DISK_DEV, unit, read);
        
        //USLOSS_DeviceInput(USLOSS_DISK_DEV, unit, &status);
        P1_WaitDevice(USLOSS_DISK_DEV, unit, &status);
        
        sectors = sectors - 1;//2;
        
        
        
        // next
        read->reg1 = (void *) (first+1);
        
        first = first + 1;//+1
        
        if(first == USLOSS_DISK_TRACK_SIZE)
        {
            if(sectors == 0)
            {
                if(DEBUG_B)
                {
                    USLOSS_Console("* Sectors is 0\n");
                }
                break;
            }
            else
            {
                if(DEBUG_B)
                {
                    USLOSS_Console("* Sectors is %d\n", sectors);
                }
               
                first = 0;
                
                //USLOSS_DeviceRequest * seekTmp = malloc(sizeof(USLOSS_DeviceRequest));
                //seekTmp->reg1 = (void *) track;
                //seekTmp->opr = USLOSS_DISK_SEEK;
                //USLOSS_DeviceOutput(USLOSS_DISK_DEV, unit, seekTmp);
                seek->reg1 = (void *) (track+1);
                seek->opr = USLOSS_DISK_SEEK;
                USLOSS_DeviceOutput(USLOSS_DISK_DEV, unit, seek);
                
                //USLOSS_DeviceInput(USLOSS_DISK_DEV, unit, &status);
                P1_WaitDevice(USLOSS_DISK_DEV, unit, &status);
                track++;
            }
        }
        
    }
    if(status == USLOSS_DEV_ERROR)
    {
        USLOSS_Console("ERROR: Disk read request error (%d) at:\nunit %d, track %d, first %d, sectors %d\n", status, unit, track, first, sectors);
    }
    if(status > 0)
    {
        P1_V(disks[unit].mutex);    // Critical section ended
        return status;
    }
    
    P1_V(disks[unit].mutex);    // Critical section ended
    
    return 0;
}




/* TODO */
/* --------------------------------------------------
 *  P2_DiskWrite()
 * --------------------------------------------------
 *  return:
 *      -1:     invalid parameters
 *       0:     sectors were read successfully
 *      >0:     diskís status register
 * -------------------------------------------------- */
int	P2_DiskWrite(int unit, int track, int first, int sectors, void *buffer)
{
    if(DEBUG_B)
    {
        USLOSS_Console("> P2_DiskWrite(): Started\n");
    }
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    
    // Check inputs
    if(!(unit >= 0 && unit < MAX_DISK)) // 0 ~ MAX_DISK
    {
        return INVALID;
    }
    if(!(track >= 0 && first >= 0))
    {
        return INVALID;
    }
    if(!(sectors > 0 && sectors < USLOSS_DISK_TRACK_SIZE))
    {
        return INVALID;
    }
    
    
    // TODO
    P1_P(disks[unit].mutex);    // Critical section started
    /*
     typedef struct USLOSS_DeviceRequest
     {
     int opr;
     void *reg1;
     void *reg2;
     } USLOSS_DeviceRequest;
     */
    
    
    /*
     opr is USLOSS_DISK_READ or USLOSS_DISK_WRITE:
     
     reg1: the index of the sector to be read or
     written within the current track
     
     reg2: a pointer to a 512-byte buffer into
     which data from the disk will be read or from
     which data will be written.
     */
    // Read from the given track
    USLOSS_DeviceRequest * write = malloc(sizeof(USLOSS_DeviceRequest));
    write->reg1 = (void *) first;
    write->reg2 = buffer;   // 512-byte buffer
    write->opr = USLOSS_DISK_WRITE;
    
    /*
     opr is USLOSS_DISK_SEEK:
     
     reg1: the track number to which the diskís
     read/write head should be moved.
     */
    // Seeking to a given track
    USLOSS_DeviceRequest * seek = malloc(sizeof(USLOSS_DeviceRequest));
    seek->reg1 = (void *) track;
    //seek->reg2 = buffer;
    seek->opr = USLOSS_DISK_SEEK;
    
    
    // Execute seek request
    //USLOSS_DeviceOutput(USLOSS_DISK_DEV,unit,request)
    USLOSS_DeviceOutput(USLOSS_DISK_DEV, unit, seek);
    
    int status = 0;
    //USLOSS_DeviceInput(USLOSS_DISK_DEV, unit, &status);
    P1_WaitDevice(USLOSS_DISK_DEV, unit, &status);
    
    
    while(status == USLOSS_DEV_READY)
    {
        // Run out of sectors and tracks
        if(sectors == 0 || first >= USLOSS_DISK_TRACK_SIZE)
        {
            P1_V(disks[unit].mutex);    // Critical section ended
            return status;
        }
        
        // Execute write request
        USLOSS_DeviceOutput(USLOSS_DISK_DEV, unit, write);
        //USLOSS_DeviceInput(USLOSS_DISK_DEV, unit, &status);
        P1_WaitDevice(USLOSS_DISK_DEV, unit, &status);
        
        
        sectors = sectors - 1;//;
        
        
        // next
        write->reg1 = (void *) (first+1);
        
        
        first = first + 1;//+1
        
        
        if(first == USLOSS_DISK_TRACK_SIZE)
        {
            if(sectors == 0)
            {
                if(DEBUG_B)
                {
                    USLOSS_Console("* Sectors is 0\n");
                }
                break;
            }
            else
            {
                if(DEBUG_B)
                {
                    USLOSS_Console("* Sectors is %d\n", sectors);
                }
                
                first = 0;
                /*
                 USLOSS_DeviceRequest * seekTmp = malloc(sizeof(USLOSS_DeviceRequest));
                 seekTmp->reg1 = (void *) track;
                 seekTmp->opr = USLOSS_DISK_SEEK;
                 USLOSS_DeviceOutput(USLOSS_DISK_DEV, unit, seekTmp);
                 */
                seek->reg1 = (void *) (track + 1);
                seek->opr = USLOSS_DISK_SEEK;
                USLOSS_DeviceOutput(USLOSS_DISK_DEV, unit, seek);
                
                //USLOSS_DeviceInput(USLOSS_DISK_DEV, unit, &status);
                P1_WaitDevice(USLOSS_DISK_DEV, unit, &status);
                track++;
            }
        }
        
    }
    
    if(status == USLOSS_DEV_ERROR)
    {
        USLOSS_Console("ERROR: Disk write request error (%d) at:\nunit %d, track %d, first %d, sectors %d\n", status, unit, track, first, sectors);
    }
    
    if(status > 0)
    {
        P1_V(disks[unit].mutex);    // Critical section ended
        return status;
    }
    
    P1_V(disks[unit].mutex);    // Critical section ended
    
    return 0;
    
}




/* TODO */
/* --------------------------------------------------
 *  P2_DiskSize()
 * --------------------------------------------------
 *  return:
 *      -1:     invalid parameters
 *       0:     sectors were read successfully
 * -------------------------------------------------- */
int P2_DiskSize(int unit, int *sector, int *track, int *disk)
{
    if(DEBUG_B)
    {
        USLOSS_Console("> P2_DiskSize(): Started\n");
    }
    
    // Check kernel mode
    if(!inKernelMode())
    {
        return INVALID;
    }
    // Check inputs
    if(!(unit >= 0 && unit < MAX_DISK)) // 0 ~ MAX_DISK
    {
        return INVALID;
    }
    
    
    // TODO
    P1_P(disks[unit].mutex);    // Critical section started
    
    /*
     opr is USLOSS_DISK_TRACKS:
     reg1: a pointer to an integer into which
     the number of disk tracks will be stored.
     */
    USLOSS_DeviceRequest * tracks = malloc(sizeof(USLOSS_DeviceRequest));
    tracks->reg1 = &disk;   // Pointer to an integer of # disk tracks will be stored
    tracks->opr = USLOSS_DISK_TRACKS;
    
    // Execute tracks request
    USLOSS_DeviceOutput(USLOSS_DISK_DEV, unit, tracks);
    //USLOSS_DeviceInput(USLOSS_DISK_DEV, unit, &status);
    int status;
    P1_WaitDevice(USLOSS_DISK_DEV, unit, &status);
    
    
    // Size of disk sector (in bytes) and number of sectors in a track
    *sector = (int *) USLOSS_DISK_SECTOR_SIZE;
    *track = (int *) USLOSS_DISK_TRACK_SIZE;
    
    P1_V(disks[unit].mutex);    // Critical section ended
    
    return 0;
}




/* --------------------------------------------------
 *  syscall_handler()
 * -------------------------------------------------- */
static void syscall_handler(int type, void *offset)
{
    if(DEBUG_A_DETAIL || DEBUG_B_DETAIL)
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
            /*
             Input:
             arg1: address of the userís line buffer
             arg2: maximum size of the buffer
             arg3: the unit number of the terminal from which to read
             
             Output:
             arg2: number of characters read
             arg4: -1 if illegal values are given as input; 0 otherwise.
             */
        case SYS_TERMREAD:
            if(DEBUG_B_DETAIL)
            {
                USLOSS_Console("* SYS_TERMREAD called\n");
            }
            
            //int P2_TermRead(int unit, int size, char *buffer)
            int xcodeTR = P2_TermRead((int) sysargs->arg3, (int) sysargs->arg2, sysargs->arg1);
            //sysargs->arg4 = (void *) Sys_TermRead((char *)sysargs->arg1, (int)sysargs->arg2, (int)sysargs->arg3, (int *)sysargs->arg2);
            
            if(xcodeTR == INVALID)
            {
                sysargs->arg4 = (void *) INVALID;
            }
            else
            {
                sysargs->arg2 = (void *) xcodeTR;
                sysargs->arg4 = (void *) 0;
            }
            
            break;
            
            
            /*
             Input:
             arg1: address of the userís line buffer
             arg2: number of characters to write
             arg3: the unit number of the terminal to which to write
             
             Output:
             arg2: number of characters written
             arg4: -1 if illegal values are given as input; 0 otherwise.
             */
        case SYS_TERMWRITE:
            if(DEBUG_B_DETAIL)
            {
                USLOSS_Console("* SYS_TERMWRITE called\n");
            }
            
            //int P2_TermWrite(int unit, int size, char *text)
            int xcodeTW = P2_TermWrite((int) sysargs->arg3, (int) sysargs->arg2, (char *) sysargs->arg1);
            //sysargs->arg4 = (void *) Sys_TermWrite((char *)sysargs->arg1, (int)sysargs->arg2, (int)sysargs->arg3, (int *)sysargs->arg2);
            
            if(xcodeTW == INVALID)
            {
                sysargs->arg4 = (void *) INVALID;
            }
            else
            {
                sysargs->arg2 = (void *) xcodeTW;
                sysargs->arg4 = (void *) 0;
            }
            
            break;
            
            
            /*
             Input:
             arg1: the memory address to which to transfer
             arg2: number of sectors to read
             arg3: the starting disk track number
             arg4: the starting disk sector number
             arg5: the unit number of the disk from which to read
             
             Output:
             arg1: 0 if transfer was successful; the disk status register otherwise.
             arg4: -1 if illegal values are given as input; 0 otherwise. The arg4 result is only set to -1 if any of the input parameters are obviously invalid, e.g. the starting sector is negative.
             */
        case SYS_DISKREAD:
            if(DEBUG_B_DETAIL)
            {
                USLOSS_Console("* SYS_DISKREAD called\n");
            }
            
            //int	P2_DiskRead(int unit, int track, int first, int sectors, void *buffer)
            int xcodeDR = P2_DiskRead((int) sysargs->arg5, (int) sysargs->arg3, (int) sysargs->arg4, (int) sysargs->arg2, (void *) sysargs->arg1);
            
            if(xcodeDR == 0)
            {
                sysargs->arg1 = (void *) 0;
                sysargs->arg4 = (void *) 0;
            }
            else if(xcodeDR == INVALID)
            {
                sysargs->arg1 = (void *) xcodeDR;
                sysargs->arg4 = (void *) INVALID;
            }
            else
            {
                sysargs->arg1 = (void *) xcodeDR;
                sysargs->arg4 = (void *) 0;
            }
            
            break;
            
            
            /*
             Input:
             arg1: the memory address from which to transfer
             arg2: number of sectors to write
             arg3: the starting disk track number
             arg4: the starting disk sector number
             arg5: the unit number of the disk to write
             
             Output:
             arg1: 0 if transfer was successful; the disk status register otherwise.
             arg4: -1 if illegal values are given as input; 0 otherwise. The arg4 result is only set to -1 if any of the input parameters are obviously invalid, e.g. the starting sector is negative.
             */
        case SYS_DISKWRITE:
            if(DEBUG_B_DETAIL)
            {
                USLOSS_Console("* SYS_DISKWRITE called\n");
            }
            
            //int	P2_DiskWrite(int unit, int track, int first, int sectors, void *buffer)
            int xcodeDW = P2_DiskRead((int) sysargs->arg5, (int) sysargs->arg3, (int) sysargs->arg4, (int) sysargs->arg2, (void *) sysargs->arg1);
            
            if(xcodeDW == 0)
            {
                sysargs->arg1 = (void *) 0;
                sysargs->arg4 = (void *) 0;
            }
            else if(xcodeDW == INVALID)
            {
                sysargs->arg1 = (void *) xcodeDW;
                sysargs->arg4 = (void *) INVALID;
            }
            else
            {
                sysargs->arg1 = (void *) xcodeDW;
                sysargs->arg4 = (void *) 0;
            }
            
            break;
            
            
            /*
             Input:
             arg1: the unit number of the disk
             
             Output:
             arg1: size of a sector, in bytes
             arg2: number of sectors in a track
             arg3: number of tracks in the disk
             arg4: -1 if illegal values are given as input; 0 otherwise.
             */
        case SYS_DISKSIZE:
            if(DEBUG_B_DETAIL)
            {
                USLOSS_Console("* SYS_DISKSIZE called\n");
            }
            
            //int P2_DiskSize(int unit, int *sector, int *track, int *disk)
            int xcodeDS = P2_DiskSize((int) sysargs->arg1, (int *) sysargs->arg1, (int *) sysargs->arg2, (int *) sysargs->arg3);
            
            if(xcodeDS == INVALID)
            {
                sysargs->arg4 = (void *) INVALID;
            }
            else    // Valid
            {
                sysargs->arg4 = (void *) 0;
            }
            
            break;
            
            
            
            
            
            
            
            
            
            /*** PHASE A ====================================================================== */
            
            
            
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
             arg5: character string containing processís name
             
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
             arg1: the processís CPU time
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
             arg1: the processís PID
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
    if(DEBUG_A_DETAIL)
    {
        USLOSS_Console("* Checking kernel mode... ");
    }
    if((USLOSS_PsrGet() & 0x1) != 1)
    {
        
        USLOSS_Console("ERROR: NOT in kernel mode!\n");
        return 0;
    }
    if(DEBUG_A_DETAIL)
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
