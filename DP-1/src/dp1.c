#include "../inc/dp1.h"
#include "../../Common/src/circularBuffer.c"
#include "../../Common/src/logger.c"
#include "../../Common/src/shm.c"
#include "../../Common/src/semaphore.c"

void cleanup(CircularBuffer *cb, int shmID );
char getRandomLetter();
void sigHandler(int sigNum);

static bool sigIntRecv = false;

int main(int argc, char *argv[])
{
    int forkReturn = 0;
    int shmID = 0;
    char arg[ARG_SIZE];
    char letter;
    key_t shmKey;
    pid_t dp2_pid, dp1_pid;
    CircularBuffer *cb = createCircularBuffer();

    // Set up signals 
    signal(SIGINT, sigHandler);

    // Initialize the seed
    srand(SEED);

    // Initialize log file
    logInit(LOG_FILE_PATH);
    logEvent(LOG_INIT_MESSAGE);
    
    // Get the shared memory key and ID
    shmKey = ftok(".", 'A');
    if((shmID = getSharedMemoryBlock(shmKey, BUFFER_SIZE)) == ERROR)
    {
        logError("Cannot get shared memory block");
    }

    // Get the semaphore
    if(getSemaphore() == ERROR)
    {
        logError("Cannot get semaphore");
        return ERROR;
    }

    if(initSemaphore() == ERROR)
    {
        logError("Cannot initialize sempahore");
        return ERROR;
    }

    // Convert the shared memory ID to a string
    sprintf(arg, "%d", shmID);
    
    // Attach the buffer to the shared memory block
    if((cb = attachSharedMemoryBlock(shmID)) == NULL)
    {
        logError("Cannot attach shared memory block");
    }
    logEvent("Attached to shared memory buffer");
    
    // Spawns DP-2
    forkReturn = fork();
    if (forkReturn == ERROR)
    {
        logError("Cannot fork");
        cleanup(cb, shmID);
        return ERROR;
    }
    // This is the child (DP-2)
    if (forkReturn == IS_CHILD)
    {
        dp2_pid = getpid();
        logEvent("Lauching the child process with PID[%d] ...", dp2_pid);

        // Call the DP-2
        logEvent("Running the DP-2 process...");
        if (execl(DP2_PATH, arg, NULL) == ERROR)
        {
            logError("Cannot run DP-2");
            cleanup(cb, shmID);
            return ERROR;
        }
    }
    // This is the parent (DP-1)
    else
    {
        // Enter the main processing loop
        while (!sigIntRecv)
        {
            logEvent("Buffer size - %d", cb->size);
            for (int i = 0; i < NUM_LETTERS; i++)
            {
                letter = getRandomLetter();
                int spaceLeft = BUFFER_SIZE - cb->size;

                // Check if read index is almost past the write index
                if((cb->writeIndex + 1) == cb->readIndex) 
                {
                    logEvent("Write index[%d] almost past the read index[%d]. Sleeping for 2 seconds ...", cb->writeIndex, cb->readIndex);
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
                // If the buffer is full
                if (writeBuffer(cb, letter) == ERROR)
                {
                    // Reset index
                    logEvent("Circular buffer is full. Resetting index ...");                   
                    cb->size = 0;
                }
                if(releaseSemaphore() == ERROR) {logError("Cannot release sempahore"); return ERROR;} // UNLOCK REGION

                logEvent("Wrote to buffer[%d]: %c", cb->writeIndex-1, letter);
            }

            // Sleep for 2 seconds
            logEvent("Sleeping for 2 seconds");
            sleep(2);
        }

        cleanup(cb, shmID);
        exit(0);
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
void cleanup(CircularBuffer *cb, int shmID )
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
