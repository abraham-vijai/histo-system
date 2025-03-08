/*
Filename    : dp2.c
Author      : Abraham Vijai
Date        : 2024-04-09
Description : This is source file for DP-2
*/

#include "../inc/dp2.h"
#include "../../Common/src/circularBuffer.c"
#include "../../Common/src/logger.c"
#include "../../Common/src/shm.c"
#include "../../Common/src/semaphore.c"

void cleanup(CircularBuffer *, int);
char getRandomLetter();
void sigHandler(int );

static bool sigIntRecv = false;

int main(int argc, char *argv[])
{

    int shmID = 0;
    int semID = 0;
    int forkReturn = 0;
    char dp2_pid_str[ARG_SIZE];
    char dp1_pid_str[ARG_SIZE];
    key_t shmKey;
    pid_t dp2_pid, dp1_pid, dc_pid;
    CircularBuffer *cb;

    // Set up signals 
    signal(SIGINT, sigHandler);

    // Initialize the seed
    srand(SEED);

    // Initialize log file
    logInit(LOG_FILE_PATH);
    logEvent(LOG_INIT_MESSAGE);

    // Set the command line arguments
    dp2_pid = getpid();
    dp1_pid = getppid();
    shmID = atoi(argv[0]);
    sprintf(dp2_pid_str, "%d", dp2_pid);
    sprintf(dp1_pid_str, "%d", dp1_pid);

    // Spawns DP-2
    forkReturn = fork();
    if (forkReturn == ERROR)
    {
        logError("Cannot fork");
        return ERROR;
    }
    // This is the child (DC)
    else if (forkReturn == IS_CHILD)
    {
        dc_pid = getpid();

        logEvent("Lauching the child process with PID[%d] ...", dc_pid);

        // Call the DP-2
        if (execl(DC_PATH, dp1_pid_str, dp2_pid_str, argv[0], NULL) == ERROR)
        {
            logError("Cannot run DC");
            return ERROR;
        }


    }
    // This is the parent (DP-2)
    else
    {
        // Waiting for the DC to launch
        logEvent("Waiting for the DC to launch...");
        usleep(50000);

        // Get the shared memory key and ID
        shmKey = ftok(".", 'A');
        if((shmID = getSharedMemoryBlock(shmKey, BUFFER_SIZE)) == ERROR)
        {
            logError("Cannot get shared memory block");
        }

        // Attach to shared memory
        if((cb = attachSharedMemoryBlock(shmID)) == NULL)
        {
            logError("Cannot attach shared memory block");
        }
        logEvent("Attached to shared memory buffer");


        // Main processing loop
        while(!sigIntRecv)
        {
            char letter = getRandomLetter();
            int spaceLeft = BUFFER_SIZE - cb->size;

            // Check is circular buffer is full
            if (isCircularBufferFull(cb))
            {
                // Reset index
                logEvent("Circular buffer is full. Resetting index ...");                   
                cb->size = 0;
            }

            // Check if write index is almost past the read index
            if((cb->writeIndex + 1) == cb->readIndex) 
            {
                logEvent("Write index[%d] almost past the read index[%d]. Sleeping for 2 seconds", cb->writeIndex, cb->readIndex);
                sleep(2);
            }
            // If space is less than 20
            if (spaceLeft < NUM_LETTERS) 
            {
                logEvent("Only %d slots available", spaceLeft);
                for (int i = 0; i < spaceLeft; i++)
                {
                    letter = getRandomLetter();
                    if(lockSemaphore() == ERROR) {logError("Cannot lock sempahore"); return ERROR;} // LOCK REGION
                    writeBuffer(cb, letter);
                    if(releaseSemaphore() == ERROR) {logError("Cannot release sempahore"); return ERROR;} // UNLOCK REGION
                    
                    logEvent("Wrote to buffer[%d]: %c", cb->writeIndex-1, letter);
                }
                
                // Reset index instead 
                if(isCircularBufferFull(cb))
                {
                    logEvent("Circular buffer is full. Resetting index ...");                   
                    cb->size = 0;
                }
                
            }
            if(lockSemaphore() == ERROR) {logError("Cannot lock sempahore"); return ERROR;} // LOCK REGION
            writeBuffer(cb, letter);
            if(releaseSemaphore() == ERROR) {logError("Cannot release sempahore"); return ERROR;} // UNLOCK REGION

            logEvent("Wrote to buffer[%d]: %c", cb->writeIndex-1, letter);

            // Sleep for 1/20 seconds
            logEvent("Sleeping for 1/20 seconds");
            usleep(50000);
        }

        cleanup(cb, shmID);
    }

    return 0;
}

/*
Method name  : getRandomLetter
Description  : Generates a random letter from a predefined list.
Parameters   : void
Return value : letters[index] - Randomly generated letter
*/
char getRandomLetter()
{
    int index = rand() % 20;

    return letters[index];
}

/*
Method name  : cleanup
Description  : Cleans up resources such as shared memory and semaphore.
Parameters   : CircularBuffer *cb - Pointer to the circular buffer.
               int shmID          - ID of the shared memory block.
Return value : void
*/
void cleanup(CircularBuffer *cb, int shmID)
{
    if (cb != NULL)
    {
        if(!detachSharedMemoryBlock(cb))
        {
            logError("Cannot detach shared memory block");
            return;
        }
        logEvent("Detached from shared memory block");
    }
    if (shmID != -1)
    {
        if(!destroySharedMemoryBlock(shmID))
        {
            logError("Cannot deallocate shared memory blockss");
            return;
        }
        logEvent("Destroyed the shared memory block");
    }
    if (semID != 0)
    {
        destroySemaphore();
        logEvent("Closed the sempahore");
    }
}

/*
Method name  : sigHandler
Description  : Signal handler for SIGINT.
Parameters   : int sigNum - Signal number
Return value : void
*/
void sigHandler(int sigNum)
{
    logEvent("SIGINT received");

    // Set SIGINT received flag to true
    sigIntRecv = true;

    signal(sigNum, sigHandler);
}