/******************************************
 * Class        : CSC-452 Phase 2 c
 * File         : phase2c.c 
 * Name         : Yang Hu / Shu Lin
 * Description  : Use USLoss to simulate Disk handler
 ******************************************/
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <usloss.h>
#include <phase1.h>
#include "phase2Int.h"

static int      DiskDriver(void *);
static void     DiskReadStub(USLOSS_Sysargs *sysargs);
static void     DiskWriteStub(USLOSS_Sysargs *sysargs);

/* if user mode (0 at 0x1) call IllegalHandler */
#define MODECHECK {\
    if (!(USLOSS_PsrGet() & USLOSS_PSR_CURRENT_MODE)) {\
        USLOSS_Console("ERROR: usermode cannot execute syscall\n");\
        P1_Quit(1024);\
    }\
}

#define DISK_MAX 2
/*
typedef struct {
	int diskID;
	int semaphoreID;
} diskInfo;
*/
typedef struct {
	int opr;
	int unit;
	int track;
	int first;
	int sectors;
	char* buffer;
} diskRequest;

int diskRun;
// deviceDriverSem: make sure deviceDriver waits for next request
// IOSmemaphore   : make sure read/write stubs doesn't return until request fullfilled
int deviceDriverSem, IOSmemaphore;
diskRequest *currRequest;

//diskInfo disks[DISK_MAX];
/*
 * P2DiskInit
 *
 * Initialize the disk data structures and fork the disk drivers.
 */
void 
P2DiskInit(void) 
{
    int rc;
    diskRun = 1;
	// initialize data structures here
	rc = P1_SemCreate("deviceDriverSem", 0, &deviceDriverSem);
	assert(rc == P1_SUCCESS);
	rc = P1_SemCreate("IOSmemaphore", 0, &IOSmemaphore);
	assert(rc == P1_SUCCESS);
	currRequest = (diskRequest*) malloc(sizeof(diskRequest));

    // install system call stubs here
    rc = P2_SetSyscallHandler(SYS_DISKREAD, DiskReadStub);
    assert(rc == P1_SUCCESS);
    rc = P2_SetSyscallHandler(SYS_DISKWRITE, DiskWriteStub);
    assert(rc == P1_SUCCESS);
    // enable interrupt
    rc = USLOSS_PsrSet(USLOSS_PsrGet() | USLOSS_PSR_CURRENT_INT);
    assert(rc == P1_SUCCESS);
    int pid;
    // fork the disk drivers here
    rc = P1_Fork("diskDriver", DiskDriver, NULL, USLOSS_MIN_STACK, 2, 0, &pid);
    assert(rc == P1_SUCCESS);
}


/*
 * P2DiskShutdown
 *
 * Clean up the disk data structures and stop the disk drivers.
 */

void 
P2DiskShutdown(void) 
{
	MODECHECK;
	int rc,status;
    // get the device status
    rc = USLOSS_DeviceInput(USLOSS_DISK_DEV, 0 ,&status);
    assert(rc == P1_SUCCESS);
    
    // stop the disk driver, the last parameter indicates abort = TRUE
    rc =P1_WakeupDevice(USLOSS_DISK_DEV, 0, status, TRUE);
    assert(rc == P1_SUCCESS);
    // stop the disk driver
	diskRun = 0;
	// Clean up the disk data structure
	free(currRequest);
}

/*
 * DiskDriver
 *
 * Kernel process that manages a disk device and services disk I/O requests from other processes.
 * Note that it may require several disk operations to service a single I/O request.
 */
static int 
DiskDriver(void *arg) 
{
	MODECHECK;
	int rc, waitStatus; //tracksRetV
	USLOSS_DeviceRequest* deviceReq = (USLOSS_DeviceRequest*)malloc(sizeof(USLOSS_DeviceRequest));
	// repeat until P2DiskShutdown has been called
	while(diskRun) {
		// wait for next request
		rc = P1_P(deviceDriverSem);
		assert(rc == P1_SUCCESS);
		
    	// which track to start at
    	int trackID = currRequest->track;
    	// if task unfinished
    	while(currRequest->sectors > 0) {
    		// do SEEK
            deviceReq->opr = USLOSS_DISK_SEEK;
            deviceReq->reg1 = &trackID;
            rc = USLOSS_DeviceOutput(USLOSS_DISK_DEV, currRequest->unit, deviceReq);
            assert(rc == P1_SUCCESS);
            // wait for SEEK result
            rc = P1_WaitDevice(USLOSS_DISK_DEV, currRequest->unit, &waitStatus);
            if(rc == P1_WAIT_ABORTED) rc = P1_SUCCESS;
            assert(rc == P1_SUCCESS);
			
            // read/write the sectors
            deviceReq->opr = currRequest->opr;
            int currSector;
            for (currSector = currRequest->first; currRequest->sectors > 0 && 
            	currSector < USLOSS_DISK_TRACK_SIZE; currSector++) {
            	/* do READ/WRITE request
            	##################################################*/
                deviceReq->reg1 = (void *) ((long) currSector);
            	deviceReq->reg2 = (void *) currRequest->buffer +
            		currSector * USLOSS_DISK_SECTOR_SIZE;
                rc = USLOSS_DeviceOutput(USLOSS_DISK_DEV, currRequest->unit, deviceReq);
                assert(rc == P1_SUCCESS);
                rc = P1_WaitDevice(USLOSS_DISK_DEV, currRequest->unit, &waitStatus);
                if(rc == P1_WAIT_ABORTED) rc = P1_SUCCESS;
                assert(rc == P1_SUCCESS);
                /*#################################################*/
                currRequest->sectors--;
                deviceReq->reg1 += USLOSS_DISK_SECTOR_SIZE;
            }
            // request first sector of next track
            trackID++;
            // next iteration: start from the 0 pos of the next sector
            currRequest->first = 0;
    	}
        //}
		// notify the caller that the request has been completed
		rc = P1_V(IOSmemaphore);
		assert(rc == P1_SUCCESS);
	}
    free(deviceReq);
    return P1_SUCCESS;
}
/*
 * DiskReadOrWriteHelper
 * 
 * reduce code redundancy caused by the similarity between P2_DiskRead & P2_DiskWrite
 */
int DiskIOHelper(int unit, int track, int first, int sectors, void *buffer, int read) {
	MODECHECK;
	// check illegal parameters
	if(unit < 0 || unit > 1) return P1_INVALID_UNIT;
	if(track < 0 || track > USLOSS_DISK_TRACK_SIZE) return P2_INVALID_TRACK;
	if(first < 0 || first > USLOSS_DISK_TRACK_SIZE) return P2_INVALID_FIRST;
	// if the planned sectors exceeds track size limit
	if( (first+sectors) / USLOSS_DISK_TRACK_SIZE + track > USLOSS_DISK_TRACK_SIZE) return P2_INVALID_SECTORS;
	if(buffer == NULL) return P2_NULL_ADDRESS;
	// read or write
    if (read) currRequest->opr = USLOSS_DISK_READ;
    else currRequest->opr = USLOSS_DISK_WRITE;

    currRequest->unit 		= unit;
    currRequest->track 		= track;
    currRequest->first 		= first;
    currRequest->sectors 	= sectors;
    currRequest->buffer 	= (char*) buffer;

    int rc = P1_V(deviceDriverSem);
    assert(rc == P1_SUCCESS);
    // wait until request finished
    rc = P1_P(IOSmemaphore);
    assert(rc == P1_SUCCESS);
    // unit = 0 or 1
    // USLOSS_DeviceOutput(USLOSS_DISK_DEV,unit,request);
    return P1_SUCCESS;
}
/*
 * P2_DiskRead
 *
 * Reads the specified number of sectors from the disk starting at the specified track and sector.
 */
int 
P2_DiskRead(int unit, int track, int first, int sectors, void *buffer) 
{
    // give request to the proper device driver
    // wait until device driver completes the request
	return DiskIOHelper(unit, track, first, sectors, buffer, 1);
}

/*
 * DiskReadStub
 *
 * Stub for the Sys_DiskRead system call.
 */
static void 
DiskReadStub(USLOSS_Sysargs *sysargs) 
{
	int rc;
	// kernel mode
	rc = USLOSS_PsrSet(USLOSS_PsrGet() | 0x1);
	assert(rc == P1_SUCCESS);

    // unpack sysargs
	int sectors = (int) sysargs->arg2;
    int track 	= (int) sysargs->arg3;
    int first 	= (int) sysargs->arg4;
    int unit 	= (int) sysargs->arg5;
    // call P2_DiskRead
    // put result in sysargs
    sysargs->arg4 = (void*) ((long)P2_DiskRead(unit, track, first, sectors, sysargs->arg1));
}

/*
 * P2_DiskWrite
 *
 * Write the specified number of sectors to the disk starting at the specified track and sector.
 */
int 
P2_DiskWrite(int unit, int track, int first, int sectors, void *buffer) 
{
    // give request to the proper device driver
    // wait until device driver completes the request
    return DiskIOHelper(unit, track, first, sectors, buffer, 0);
}

/*
 * DiskWriteStub
 *
 * Stub for the Sys_DiskWrite system call.
 */
static void 
DiskWriteStub(USLOSS_Sysargs *sysargs) 
{
    int rc;
	// kernel mode
	rc = USLOSS_PsrSet(USLOSS_PsrGet() | 0x1);
	assert(rc == P1_SUCCESS);

    // unpack sysargs
	int sectors = (int) sysargs->arg2;
    int track 	= (int) sysargs->arg3;
    int first 	= (int) sysargs->arg4;
    int unit 	= (int) sysargs->arg5;
    // call P2_DiskRead
    // put result in sysargs
    sysargs->arg4 = (void*) ((long)P2_DiskWrite(unit, track, first, sectors, sysargs->arg1));
    
}
