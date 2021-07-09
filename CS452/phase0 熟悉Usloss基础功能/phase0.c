/******************************************
 * Class		: CSC-452 Phase 0
 * File 		: phase0.c 
 * Name 		: Yang Hu
 * Description	: Use USLoss to print out repeating "Hello World"s
 ******************************************/

#include <stdio.h>
#include "usloss.h"

/* Global Variables */
USLOSS_Context context0;
USLOSS_Context context1;

#define SIZE (USLOSS_MIN_STACK * 2)

char stack0[SIZE];
char stack1[SIZE];

/* Context "hello" */
void
Test0(void)
{
    int i;
    for (i = 0; i < 10; i++) {
        USLOSS_Console("%d Hello ", i+1);
        USLOSS_ContextSwitch(&context0, &context1);
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
        USLOSS_ContextSwitch(&context1, &context0);
    }
    USLOSS_Halt(0);
}

void
startup(int argc, char **argv)
{
    USLOSS_ContextInit(&context0, stack0, sizeof(stack0), NULL, Test0);
    USLOSS_ContextInit(&context1, stack1, sizeof(stack1), NULL, Test1);
    // start from the "hello" context
    USLOSS_ContextSwitch(NULL, &context0);
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

