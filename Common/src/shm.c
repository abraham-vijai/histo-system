/*
Filename    : shm.c
Author      : Abraham Vijai
Date        : 2024-04-09
Description : This file contains basic shared memory functions
*/

#include "../inc/shm.h"
#include "../inc/logger.h"

/*
Function     : getSharedMemoryBlock
Description  : Creates or retrieves an existing shared memory block.
Parameters   : shmKey - The key used to identify the shared memory block.
               shmSize - The size of the shared memory block.
Return value : shmID - The ID of the shared memory block if successful
               ERROR - In case of error
*/
int getSharedMemoryBlock(key_t shmKey, int shmSize) 
{
    int shmID = shmget(shmKey, shmSize, IPC_CREAT | 0666);
    if (shmID == ERROR) 
    {
        return ERROR;
    }

    return shmID;
}

/*
Function     : attachSharedMemoryBlock
Description  : Attaches to an existing shared memory block.
Parameters   : shmID - The ID of the shared memory block to attach to.
Return value : cb   - A pointer to the attached CircularBuffer struct 
               NULL - In case of error.
*/
CircularBuffer *attachSharedMemoryBlock(int shmID) 
{
    CircularBuffer *cb = shmat(shmID, NULL, 0);
    if (cb == (void *)ERROR) 
    {
        return NULL;
    }
    
    return cb;
}

/*
Function     : detachSharedMemoryBlock
Description  : Detaches from a shared memory block.
Parameters   : cb - A pointer to the CircularBuffer struct representing the shared memory block.
Return value : true  - If the shared memory is detached successfuly
               false - In case of error
*/
bool detachSharedMemoryBlock(CircularBuffer *cb) 
{
    if (shmdt(cb) == ERROR) 
    {
        return false;
    }

    return true;
}

/*
Function     : destroySharedMemoryBlock
Description  : Destroys a shared memory block.
Parameters   : shmID - The ID of the shared memory block to destroy.
Return value : true  - If the shared memory is destroyed successfuly
               false - In case of error
*/
bool destroySharedMemoryBlock(int shmID) 
{
    if (shmctl(shmID, IPC_RMID, NULL) == ERROR) 
    {
        return false;
    }
    
    return true;
}