/******************************************
 * Class        : CSC-452 Phase 1 a
 * File         : phase1a.c 
 * Name         : Yang Hu / Shu Lin
 * Description  : Use USLoss to simulate simple os
 ******************************************/

#include "phase1Int.h"
#include "usloss.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

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

extern  USLOSS_PTE  *P3_AllocatePageTable(int cid);
extern  void        P3_FreePageTable(int cid);

typedef struct Context {
    void            (*startFunc)(void *);
    void            *startArg;
    USLOSS_Context  context;
    int             occupied;
    char            *contextStack;
    // you'll need more stuff here
} Context;

static Context   contexts[P1_MAXPROC];

static int currentCid = -1;

/* if failed exception return true*/
#define checkReturn(x) \
    if( x == USLOSS_ERR_INVALID_PSR ) {\
        USLOSS_Console("Invalid PSR value.");\
        USLOSS_Halt(1);}

/* if user mode (0 at 0x1) return true */
#define checkMode() \
    if(!(USLOSS_PsrGet() & USLOSS_PSR_CURRENT_MODE)){\
        USLOSS_Console("Invalid Mode: User Mode");\
        USLOSS_Halt(1);}

/*
 * Helper function to call func passed to P1ContextCreate with its arg.
 */
static void launch(void)
{
    assert(contexts[currentCid].startFunc != NULL);
    contexts[currentCid].startFunc(contexts[currentCid].startArg);
}

void P1ContextInit(void)
{
    // enter kernel mode
    checkReturn(USLOSS_PsrSet(USLOSS_PsrGet() | 0x1));
    checkMode();
}

int P1ContextCreate(void (*func)(void *), void *arg, int stacksize, int *cid) {
    checkMode();
    /* P1_INVALID_STACK */
    if (stacksize < USLOSS_MIN_STACK)
    {
        return P1_INVALID_STACK;
    }

    int result = P1_SUCCESS;
    int newContextID;
    /* Find unused struct */
    for (int i = 0; i < P1_MAXPROC; ++i)
    {
        if (!contexts[i].occupied)
        {
            newContextID = i;
            contexts[i].occupied = 1;
            break;
        }
        /* P1_TOO_MANY_CONTEXTS */
        if (i == P1_MAXPROC - 1)
        {
            return P1_TOO_MANY_CONTEXTS;
        }
    }

    *cid = newContextID;
    /* Initialize new context struct content*/
    contexts[newContextID].startFunc      = func;
    contexts[newContextID].startArg       = arg;
    contexts[newContextID].contextStack   = malloc(stacksize);
    /* Initialize new context */
    USLOSS_ContextInit(&contexts[newContextID].context, contexts[newContextID].contextStack, 
        stacksize, P3_AllocatePageTable(newContextID), launch);

    return result;
}

int P1ContextSwitch(int cid) {
    checkMode();
    USLOSS_Context *formerContext;
    // if first context
    if (currentCid < 0)
    {
        formerContext = NULL;
        currentCid = 0;
    } 
    // if cid is not valid
    else if (!contexts[cid].occupied)
    {
        return P1_INVALID_CID;
    } 
    else 
    {
        formerContext = &(contexts[currentCid].context);
    }
    currentCid = cid;

    USLOSS_ContextSwitch(formerContext, &(contexts[cid].context));

    return P1_SUCCESS;
}

int P1ContextFree(int cid) {
    checkMode();
    int result = P1_SUCCESS;
    // if cid is not valid
    if (!contexts[cid].occupied)
    {
        return P1_INVALID_CID;
    }
    /* Free allocated stack */
    free(contexts[cid].contextStack);

    P3_FreePageTable(cid);
    // free the stack and mark the context as unused
    // P1ContextFree should call P3_FreePageTable to free the page table
    return result;
}


void 
P1EnableInterrupts(void) 
{
    checkMode();
    // set the interrupt bit in the PSR
    checkReturn(USLOSS_PsrSet(USLOSS_PsrGet() | USLOSS_PSR_CURRENT_INT));
}

/*
 * Returns true if interrupts were enabled, false otherwise.
 */
int 
P1DisableInterrupts(void) 
{
    int enabled = (USLOSS_PsrGet() & USLOSS_PSR_CURRENT_INT) >> 1;
    checkReturn(USLOSS_PsrSet(USLOSS_PsrGet() & ~USLOSS_PSR_CURRENT_INT));
    // set enabled to TRUE if interrupts are already enabled
    // clear the interrupt bit in the PSR
    return enabled;
}