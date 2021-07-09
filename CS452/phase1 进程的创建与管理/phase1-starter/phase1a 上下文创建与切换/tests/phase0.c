/******************************************
 * Class        : CSC-452 Phase 0
 * File         : phase0.c 
 * Name         : Yang Hu
 * Description  : Use USLoss to print out repeating "Hello World"s
 ******************************************/

#include <stdio.h>
#include "usloss.h"
#include <phase1.h>
#include <phase1Int.h>
#include <assert.h>

/* Global Variables */
USLOSS_Context context0;
USLOSS_Context context1;

#define SIZE (USLOSS_MIN_STACK)

char stack0[SIZE];
char stack1[SIZE];
int cid1, cid2;

/* Context "hello" */
void
Test0(void)
{
    int i;
    for (i = 0; i < 10; i++) {
        USLOSS_Console("%d Hello ", i+1);
        P1ContextSwitch(cid2);
    }
    USLOSS_Halt(0);
}

/* Context "world" */
void
Test1(void)
{
    int i;
    for (i = 0; i < 10; i++) {
        /* asterisk (*) can pass the str_len specifier/precision to printf()
           and print out the target_str in certain str_len
           note: str_len should NOT be larger than the length of the target_str
                 in this case, 10 "!" means at most 10 '!' can be printed
           printf("%.*s\n", str_len, target_str);
        */ 
        USLOSS_Console("World%.*s\n", i+1, "!!!!!!!!!!");
        P1ContextSwitch(cid1);
    }
    USLOSS_Halt(0);
}

void
startup(int argc, char **argv)
{
    int rc;
    P1ContextInit();

    rc = P1ContextCreate(Test0, NULL, USLOSS_MIN_STACK, &cid1);

    assert(rc == P1_SUCCESS);

    rc = P1ContextCreate(Test1, NULL, USLOSS_MIN_STACK, &cid2);
    assert(rc == P1_SUCCESS);


    rc = P1ContextSwitch(cid1);
    assert(rc == P1_SUCCESS);

}

// Do not modify anything below this line.

void
finish(int argc, char **argv)
{
    USLOSS_Console("Goodbye.\n");
}

void
test_setup(int argc, char **argv)
{
    // Do nothing.
}

void
test_cleanup(int argc, char **argv)
{
    // Do nothing.
}

