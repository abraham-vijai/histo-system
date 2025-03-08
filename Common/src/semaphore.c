/*
Filename    : semaphore.c
Author      : Abraham Vijai
Date        : 2024-04-09
Description : This file contains basic semaphore functions
*/

#include "../inc/semaphore.h"

static int semID = 0;

/*
Method name  : getSemaphore
Description  : Create a new semaphore set and return its ID.
Parameters   : None
Return value : int - Semaphore ID on success, ERROR (-1) on failure
*/
int getSemaphore()
{
    semID = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);

    if(semID == ERROR)
    {
        return ERROR;
    }

    return 0;
}

/*
Method name  : initSemaphore
Description  : Initialize the semaphore set.
Parameters   : None
Return value : int - 0 on success, ERROR (-1) on failure
*/
int initSemaphore()
{
    if(semctl(semID, 0, SETALL, init_values) == ERROR)
    {
        return ERROR;
    }
}

/*
Method name  : lockSemaphore
Description  : Lock the semaphore.
Parameters   : None
Return value : int - 0 on success, ERROR (-1) on failure
*/
int lockSemaphore()
{
    if (semop (semID, &acquire_operation, 1) == ERROR)
    {
        return ERROR;
    } 
}

/*
Method name  : releaseSemaphore
Description  : Release the semaphore.
Parameters   : None
Return value : int - 0 on success, ERROR (-1) on failure
*/
int releaseSemaphore()
{
    if (semop (semID, &release_operation, 1) == ERROR)
    {
        return ERROR;
    } 
}

/*
Method name  : destroySemaphore
Description  : Deallocate the semaphore set.
Parameters   : None
Return value : int - 0 on success, ERROR (-1) on failure
*/
int destroySemaphore() 
{

    if(semctl(semID, 0, IPC_RMID) == ERROR)
    {
        return ERROR;
    }
    return 0;
}
