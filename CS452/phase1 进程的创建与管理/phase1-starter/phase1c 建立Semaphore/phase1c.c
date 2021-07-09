/******************************************
 * Class        : CSC-452 Phase 1 c
 * File         : phase1v.c 
 * Name         : Yang Hu / Shu Lin
 * Description  : Use USLoss to simulate simple os (Semaphore functionalities)
 ******************************************/
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include "usloss.h"
#include "phase1Int.h"

extern int P1SetState(int pid, P1_State state, int sid);
extern int P1_GetPID();
extern void P1Dispatch(int rotate);

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

#define checkSid() {\
        if (sid < 0 || sid > P1_MAXSEM) {\
            USLOSS_Console("Error Sid.\n");\
            return P1_INVALID_SID;}}

/* the MACRO restoreInt relies on disableInt on variable wasEnabled */
#define disableInt() \
        int wasEnabled = (USLOSS_PsrGet() & 0x2) >> 1;\
        checkReturn(USLOSS_PsrSet(USLOSS_PsrGet() & ~0x2));

#define restoreInt() \
        if(wasEnabled) checkReturn(USLOSS_PsrSet(USLOSS_PsrGet() | 0x2));
    
/* Queue for semaphore */
struct LinkedList{
    int pid;
    struct LinkedList* nextNode;
};
typedef struct LinkedList* Node;

/* Semaphore */
typedef struct Sem
{
    char        name[P1_MAXNAME+1];
    u_int       value;
    int         occupied;
    int         isWaited;
    Node        queue;
    // more fields here
} Sem;

static Sem sems[P1_MAXSEM];

/* -------------- Queue functions ------------- */
Node createNode(int pid){
    Node node = (Node) malloc(sizeof(struct LinkedList));
    node->nextNode = NULL;
    node->pid = pid;
    return node;
}
void push(int pid, int sid){
    Node newNode = createNode(pid);
    Node tmpNode = sems[sid].queue;
    if (sems[sid].queue == NULL) {
        sems[sid].queue = newNode;
    }
    else {
        while(tmpNode->nextNode != NULL) {
            if(tmpNode->pid == pid) return;
            tmpNode = tmpNode->nextNode;
        }
        tmpNode->nextNode = newNode;
    }
}
int pop(int sid){
    if (sems[sid].queue == NULL) return -1;
    Node tmp = sems[sid].queue;
    sems[sid].queue = sems[sid].queue->nextNode;
    int pid = tmp->pid;
    free(tmp);
    return pid;
}
/* ------------- Queue functions End ------------ */

void 
P1SemInit(void) 
{
    P1ProcInit();
    checkMode(); // check for kernel mode
    // initialize sems here
    for (int i = 0; i < P1_MAXSEM; ++i)
    {
        sems[i].value = 0;
        sems[i].occupied = 0;
    }
}

int P1_SemCreate(char *name, unsigned int value, int *sid)
{
    checkMode(); // check for kernel mode
    // check parameters
    // P1_NAME_IS_NULL : name is NULL
    if (name == NULL) return P1_NAME_IS_NULL;
    // P1_NAME_TOO_LONG: name is longer than P1_MAXNAME
    if (strlen(name) > P1_MAXNAME) return P1_NAME_TOO_LONG;

    int emptyPos = -1;

    // disable interrupts
    disableInt();

    for (int i = 0; i < P1_MAXNAME; ++i)
    {
        if (!sems[i].occupied && emptyPos < 0) emptyPos = i;
        
        // P1_DUPLICATE_NAME : name already in use
        if(strcmp(sems[i].name, name) == 0) {
            return P1_DUPLICATE_NAME;
        }
    }
    // P1_TOO_MANY_SEMS: no more semaphores
    if(emptyPos < 0) return P1_TOO_MANY_SEMS;

    // find a free Sem and initialize it
    strcpy(sems[emptyPos].name, name);
    sems[emptyPos].value = value;
    *sid = emptyPos;

    // re-enable interrupts if they were previously enabled
    restoreInt();
    return P1_SUCCESS;
}

int P1_SemFree(int sid) 
{
    checkMode(); // check for kernel mode
    checkSid(); // check if sid is valid
    
    // P1_BLOCKED_PROCESSES: processes are blocked on the semaphore
    if (sems[sid].isWaited == 1)
    {
        return P1_BLOCKED_PROCESSES;
    }
    return P1_SUCCESS;
}
/* adapted from slide sudo code */
int P1_P(int sid) 
{
    checkMode(); // check for kernel mode
    checkSid(); // check if sid is valid

    int currPid = P1_GetPid();
    // while value == 0
    //      set state to P1_STATE_BLOCKED

    while(1) {
        disableInt(); // disable interrupts
        if(sems[sid].value > 0) {
            sems[sid].value--;
            break;
        }

        // remove process from ready queue
        P1SetState(currPid, P1_STATE_BLOCKED, sid);

         // add current pid to semaphore (sid)'s waiting queue
        push(currPid, sid);
        
        restoreInt();
        P1Dispatch(0);
    }
    return P1_SUCCESS;
}
/* adapted from slide sudo code */
int P1_V(int sid) 
{
    checkMode(); // check for kernel mode
    checkSid(); // check if sid is valid
    disableInt(); // disable interrupts
    // value++
    sems[sid].value++;
    // if a process is waiting for this semaphore
    //      set the process's state to P1_STATE_READY
    int waitingPid = pop(sid);
    if(waitingPid >= 0) P1SetState(waitingPid, P1_STATE_READY, sid);
    P1Dispatch(0);
    // re-enable interrupts if they were previously enabled
    restoreInt();
    return P1_SUCCESS;
}

int P1_SemName(int sid, char *name) {
    checkMode(); // check for kernel mode
    checkSid(); // check if sid is valid
    if (sems[sid].name == NULL) return P1_NAME_IS_NULL;
    // return name
    strcpy(name, sems[sid].name);
    return P1_SUCCESS;
}
