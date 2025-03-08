#include "../inc/dc.h"
#include "../../Common/src/circularBuffer.c"
#include "../../Common/src/logger.c"
#include "../../Common/src/shm.c"
#include "../../Common/src/semaphore.c"

void sigHandler(int sigNum);
void initStruct(KeyValuePair kvp[]);
void incrementCount(KeyValuePair kvp[], char letter);
void isElapsed(int *secondsElapsed, int increment, KeyValuePair kvp[]);
void printHistogram(char letter, int count);
void cleanup(CircularBuffer *, int);

static bool isElapsed_2 = false;
static bool sigIntRecv = false;

int main(int argc, char *argv[])
{
    int shmID = 0;
    int secondsElapsed = 0;
    char letter;
    void *threadReturn;
    pid_t dp1_pid, dp2_pid;
    CircularBuffer *cb;
    KeyValuePair kvp[NUM_LETTERS];

    // Read and store the command line arguments
    dp1_pid = atoi(argv[0]);
    dp2_pid = atoi(argv[1]);
    shmID = atoi(argv[2]);

    // Set up signals 
    signal(SIGINT, sigHandler);
    signal(SIGALRM, sigHandler);

    // Set up alarm to for 2 seconds
    alarm(WAKE_UP_INTERVAL);

    // Initialize log file
    logInit(LOG_FILE_PATH);
    logEvent(LOG_INIT_MESSAGE);
    
    // Initialize struct
    initStruct(kvp);

    // Attach to shared memory
    if((cb = attachSharedMemoryBlock(shmID)) == NULL)
    {
        logError("Cannot attach shared memory block");
    }
    logEvent("Attached to shared memory buffer"); 
    
    // Main processing loop
    while(1)
    {

        // Check if 2 seconds have elapsed, if so read from the buffer
        if(isElapsed_2)
        {
            // Reset the flag
            isElapsed_2 = false;

            logEvent("2 seconds elapsed. Reading from the buffer...");
            
            // Increment elapsed seconds 
            isElapsed(&secondsElapsed, 1, kvp);
            isElapsed(&secondsElapsed, 1, kvp);
            
            for (int i = 0; i < cb->writeIndex; i++)
            {
                // Read from buffer
                if(lockSemaphore() == ERROR) {logError("Cannot lock sempahore"); return ERROR;} // LOCK REGION
                letter = readBuffer(cb);
                if(releaseSemaphore() == ERROR) {logError("Cannot release sempahore"); return ERROR;} // UNLOCK REGION

                if(letter == ERROR)
                {
                    logError("Cannot read from empty buffer");
                    sleep(2);
                }
                else
                {
                    logEvent("Read from buffer[%d]: %c",cb->readIndex-1, letter);

                    // Increment count of the letter
                    incrementCount(kvp, letter);
                }

                // If the read index catches upto write index
                if((cb->readIndex + 1) == cb->writeIndex)
                {
                    logEvent("Read index[%d] almost past the write index[%d]. Sleeping for 1 second", cb->readIndex, cb->writeIndex); //d or maybe more
                    sleep(1);

                    // Increment elapsed seconds
                    isElapsed(&secondsElapsed, 1, kvp);
                    
                    if(isElapsed_2)
                    {
                        // Increment elapsed seconds
                        isElapsed(&secondsElapsed, -1, kvp);
                        isElapsed(&secondsElapsed, -1, kvp);
                        break;
                    }
                }
            }
        }

        // Check if SIGINT signal is received
        if(sigIntRecv)
        {
            // Send the SIGINT to DP-1 and DP-2
            logEvent("Sending SIGINT to DP-1[%d] and DP-2[%d]", dp1_pid, dp2_pid);
            kill(dp1_pid, SIGINT);
            kill(dp2_pid, SIGINT);

            // Read until read index catches up to write index
            logEvent("Reading unitl read index[%d] catches up with write index[%d]", cb->readIndex, cb->writeIndex);
            while(cb->readIndex != cb->writeIndex)
            {
                // Read from buffer
                if(lockSemaphore() == ERROR) {logError("Cannot lock sempahore"); return ERROR;} // LOCK REGION
                char letter = readBuffer(cb);
                if(releaseSemaphore() == ERROR) {logError("Cannot release sempahore"); return ERROR;} // UNLOCK REGION
                
                logEvent("Read from buffer[%d]: %c",cb->readIndex-1, letter);

                // Increment count of the letter
                incrementCount(kvp, letter); 
            }

            // Clear the output screen
            system("clear");

            // Print the histogram
            for (int i = 0; i < NUM_LETTERS; i++)
            {
                printHistogram(kvp[i].key, kvp[i].value);
            }

            // Print the exit statement
            printf(EXIT_STATEMENT);
            
            // Cleanup IPCs
            cleanup(cb, shmID);

            

            exit(0);
        }
    
    }

    return 0;
}

/*
Method name  : sigHandler
Description  : Signal handler for SIGINT and SIGALRM.
Parameters   : int sigNum - Signal number
Return value : void
*/
void sigHandler(int sigNum)
{
    switch (sigNum)
    {
        case SIGINT:

            logEvent("SIGINT received");

            // Set flag to true            
            sigIntRecv = true;

            // Re-regsiter the signal handler
            signal(sigNum, sigHandler);   
            break;
        
        case SIGALRM:
            logEvent("SIGALRM received");
            
            // Set flag to true
            isElapsed_2 = true;

            // Reschedule the alarm
            alarm(2);
            
            // Re-regsiter the signal handler
            signal(sigNum, sigHandler);        
            break;
        
        default:
            break;
    }
}

/*
Method name  : initStruct
Description  : Initialize the key-value pairs in the array.
Parameters   : KeyValuePair kvp[] - Array of KeyValuePair structures
Return value : void
*/
void initStruct(KeyValuePair kvp[])
{
    for (int i = 0; i < NUM_LETTERS; i++)
    {
        kvp[i].key = 'A' + i;
        kvp[i].value = 0;
    }

    return;
}

/*
Method name  : incrementCount
Description  : Increment the count of a letter in the key-value pair array.
Parameters   : KeyValuePair kvp[] - Array of KeyValuePair structures
               char letter - Letter to increment count for
Return value : void
*/
void incrementCount(KeyValuePair kvp[], char letter)
{
    for (int i = 0; i < NUM_LETTERS; i++)
    {
        if (kvp[i].key == letter)
        {
            kvp[i].value++;
            break;
        }
    }
}

/*
Method name  : isElapsed
Description  : Increment the second counter
Parameters   : int *secondsElapsed - Pointe to the secondsElapsed integer
               int increment       - Value to be increment by
               KeyValuePair kvp[]  - Array of KeyValuePair structures
Return value : void
*/
void isElapsed(int *secondsElapsed, int increment, KeyValuePair kvp[])
{
    *secondsElapsed += increment;

    if (*secondsElapsed == PRINT_INTERVAL)
    {
        logEvent("10 seconds elapsed. Printing histogram...");

        // Clear the output screen
        system("clear");

        // Print the histogram
        for (int i = 0; i < NUM_LETTERS; i++)
        {
            printHistogram(kvp[i].key, kvp[i].value);
        }

        // Reset the timer
        *secondsElapsed = 0;
    }
}

/*
Method name  : printHistogram
Description  : Print the histogram for a letter.
Parameters   : char letter - Letter
               int count   - Count of occurrences
Return value : void
*/
void printHistogram(char letter, int count)
{
    int ones, tens, hundreds = 0;

    printf("%c: ", letter);

    if (count < 10)
    {

        for (int i = 0; i < count; i++)
        {
            printf("-");
        }
        printf("\n");
    }

    else if (count >= 10 && count < 100)
    {
        tens = count / 10;
        ones = count - (tens * 10);

        for (int i = 0; i < tens; i++)
        {
            printf("+");
        }
        for (int i = 0; i < ones; i++)
        {
            printf("-");
        }
        printf("\n");
    }

    else
    {
        hundreds = count / 100;
        tens = count / 10 - (hundreds * 10);
        ones = count - (hundreds * 100) - (tens * 10);

        for (int i = 0; i < hundreds; i++)
        {
            printf("*");
        }
        for (int i = 0; i < tens; i++)
        {
            printf("+");
        }
        for (int i = 0; i < ones; i++)
        {
            printf("-");
        }
        printf("\n");
    }
}

/*
Method name  : cleanup
Description  : Cleanup function to release resources such as circular buffer and shared memory.
Parameters   : CircularBuffer *cb - Pointer to the circular buffer
               int shmID          - ID of the shared memory block
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
            logError("Cannot deallocate shared memory blocks");
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