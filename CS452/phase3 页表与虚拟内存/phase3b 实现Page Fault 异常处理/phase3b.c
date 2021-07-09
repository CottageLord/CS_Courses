/******************************************
 * Class        : CSC-452 Phase 3 b
 * File         : phase3b.c 
 * Name         : Yang Hu / Shu Lin
 * Description  : Phase 3b implements a page fault interrupt handler that 
 *                fills in the PTE for the offending page with the identity mapping.
 ******************************************/

#include <assert.h>
#include <phase1.h>
#include <phase2.h>
#include <usloss.h>
#include <string.h>
#include <libuser.h>
#include "phase3Int.h"

void
P3PageFaultHandler(int type, void *arg)
{
    /*
    USLOSS_MMU_FAULT means that the page is not mapped to a frame. 
    USLOSS_MMU_ACCESS means that a mapping was found, 
        but that the protections on the page prohibit the attempted access.
    USLOSS_MMU_NONE means that an MMU interrupt has not yet occurred.
    */
    USLOSS_PTE  *table = NULL;
    int rc;
    int err = USLOSS_MmuGetCause();
    // if the cause is USLOSS_MMU_FAULT
    if (err == USLOSS_MMU_FAULT)
    {
        rc = P3PageTableGet(P1_GetPid(), &table);
        assert(rc == P1_SUCCESS);
        // if the process does not have a page table
        if (table == NULL)
        {
            USLOSS_Console("The process does not have a page table.\n");
            USLOSS_Halt(1);
        }
        /*
        determine which page suffered the fault (USLOSS_MmuPageSize)
        update the page's PTE to map page x to frame x
        set the PTE to be read-write and incore
        update the page table in the MMU (USLOSS_MmuSetPageTable)
        */
        else
        {
            int size = USLOSS_MmuPageSize();
            int offset = (int) arg;
            int index = offset / size;

            table[index].read = 1;
            table[index].write = 1;
            table[index].incore = 1;

            rc = USLOSS_MmuSetPageTable(table);
            assert(rc == USLOSS_MMU_OK);
        }
    }
    else 
    {
        USLOSS_Console(".\n");
        USLOSS_Halt(1);
    }
}

USLOSS_PTE *
P3PageTableAllocateEmpty(int pages)
{
    USLOSS_PTE  *table = NULL;
    // allocate and initialize an empty page table
    
    table = malloc(pages * sizeof(USLOSS_PTE));
    // return NULL if table could not be allocated
    // pointer to the table otherwise.
    if (table == NULL) return NULL;

    for (int i = 0; i < pages; ++i)
    {
        table[i].incore = 0;
        table[i].read = 0;
        table[i].write = 0;
        table[i].frame = i;
    }
    
    return table;
}
