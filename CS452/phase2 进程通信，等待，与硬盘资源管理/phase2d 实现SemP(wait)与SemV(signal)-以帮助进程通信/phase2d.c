/******************************************
 * Class        : CSC-452 Phase 2 d
 * File         : phase2d.c 
 * Name         : Yang Hu / Shu Lin
 * Description  : The init program for phase 2
 ******************************************/

#include <string.h>
#include <stdlib.h>
#include <usloss.h>
#include <phase1.h>
#include <assert.h>
#include <libuser.h>
#include <libdisk.h>
#include "phase2Int.h"

static void     CreateStub(USLOSS_Sysargs *sysargs);
static void     SemPStub(USLOSS_Sysargs *sysargs);
static void     SemVStub(USLOSS_Sysargs *sysargs);
static void     FreeStub(USLOSS_Sysargs *sysargs);
static void     NameStub(USLOSS_Sysargs *sysargs);

/*
 * I left this useful function here for you to use for debugging. If you add -DDEBUG to CFLAGS
 * this will produce output, otherwise it won't. The message is printed including the function
 * and line from which it was called. I don't remember why it is called "debug2".
 */
static void debug2(char *fmt, ...)
{
    #ifdef DEBUG
    va_list ap;
    va_start(ap, fmt);
    USLOSS_Console("[%s:%d] ", __PRETTY_FUNCTION__, __LINE__);   \
    USLOSS_VConsole(fmt, ap);
    #endif
}

int P2_Startup(void *arg)
{
    int rc, pid;
    // initialize clock and disk drivers
    P2ClockInit();
    P2DiskInit();
    debug2("starting\n");

    // install system calls
    rc = P2_SetSyscallHandler(SYS_SEMCREATE, CreateStub);
    assert(rc == P1_SUCCESS);
    rc = P2_SetSyscallHandler(SYS_SEMP, SemPStub);
    assert(rc == P1_SUCCESS);
    rc = P2_SetSyscallHandler(SYS_SEMV, SemVStub);
    assert(rc == P1_SUCCESS);
    rc = P2_SetSyscallHandler(SYS_SEMFREE, FreeStub);
    assert(rc == P1_SUCCESS);
    rc = P2_SetSyscallHandler(SYS_SEMNAME, NameStub);
    assert(rc == P1_SUCCESS);

    // create P3_Startup process
    rc = P2_Spawn("P3_Startup", P3_Startup, NULL, 4 * USLOSS_MIN_STACK, 3, &pid);
    assert(rc == P1_SUCCESS);

    int waitPid, status;
    rc = P2_Wait(&waitPid, &status);
    assert(rc == P1_SUCCESS);
    // shut down clock and disk drivers
    P2ClockShutdown();
    P2DiskShutdown();
    return 0;
}

static void
CreateStub(USLOSS_Sysargs *sysargs)
{
    sysargs->arg4 = (void *) P1_SemCreate((char *) sysargs->arg2, (int) sysargs->arg1, 
                                          (void *) &sysargs->arg1);
}

static void     
SemPStub(USLOSS_Sysargs *sysargs)
{
    sysargs->arg4 = (void *) P1_P((int)sysargs->arg1);
}

static void     
SemVStub(USLOSS_Sysargs *sysargs)
{
    sysargs->arg4 = (void *) P1_V((int)sysargs->arg1);
}

static void     
FreeStub(USLOSS_Sysargs *sysargs)
{
    sysargs->arg4 = (void *) P1_SemFree((int)sysargs->arg1);
}

static void     
NameStub(USLOSS_Sysargs *sysargs)
{
    sysargs->arg4 = (void *) P1_SemName((int)sysargs->arg1, (char *)sysargs->arg2);
}