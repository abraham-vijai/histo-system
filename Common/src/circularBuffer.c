/*
Filename    : circularBuffer.c
Author      : Abraham Vijai
Date        : 2024-04-09
Description : This file contains the functions for the circular buffer
*/

#include "../inc/circularBuffer.h"

/*
Function     : createCircularBuffer
Description  : Creates and initializes a circular buffer.
Parameters   : void
Return value : A pointer to the newly created CircularBuffer struct.
*/
CircularBuffer* createCircularBuffer() 
{
    CircularBuffer *cb = (CircularBuffer*)malloc(sizeof(CircularBuffer));
    cb->readIndex = 0;  // Read index
    cb->writeIndex = 0; // Write index
    cb->size = 0;       // Current size
    
    return cb;
}

/*
Function     : isCircularBufferEmpty
Description  : Checks if the circular buffer is empty.
Parameters   : cb - A pointer to the CircularBuffer struct to check.
Return value : true  - If the buffer is empty
               false - If the buffer is not empty
*/
bool isCircularBufferEmpty(CircularBuffer *cb) 
{
    if(cb->size == 0)
    {
        return true;
    }

    return false;
}

/*
Function     : isCircularBufferFull
Description  : Checks if the circular buffer is full.
Parameters   : cb - A pointer to the CircularBuffer struct to check.
Return value : true  - If the buffer is full
               false - If the buffer is not full
*/
bool isCircularBufferFull(CircularBuffer *cb) 
{
    if(cb->size == BUFFER_SIZE)
    {
        return true;
    }

    return false;
}

/*
Function     : writeBuffer
Description  : Writes a value into the circular buffer.
Parameters   : cb    - A pointer to the CircularBuffer struct to write into.
               value - The value to write into the buffer.
Return value : ERROR - If the buffer is empty.
               0     - If successful
*/
int writeBuffer(CircularBuffer *cb, int value) 
{
    if (isCircularBufferFull(cb)) 
    {
        return ERROR;
    }

    cb->buffer[cb->writeIndex] = value;                     // Insert value at write index
    cb->writeIndex = (cb->writeIndex + 1) % BUFFER_SIZE;    // Increment the write value and keep it in range
    cb->size++;                                             // Increment the size

    return 0;
}

/*
Function     : readBuffer
Description  : Reads a value from the circular buffer.
Parameters   : cb - A pointer to the CircularBuffer struct to read from.
Return value : ERROR - If the buffer is empty.
               value - The value read from the buffer
*/
char readBuffer(CircularBuffer *cb) 
{
    if (isCircularBufferEmpty(cb)) 
    {
        return ERROR;
    }

    int value = cb->buffer[cb->readIndex];
    cb->readIndex = (cb->readIndex + 1) % BUFFER_SIZE;

    return value;
}


