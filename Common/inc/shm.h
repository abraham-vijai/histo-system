/*
Filename    : shm.h
Author      : Abraham Vijai
Date        : 2024-04-09
Description : Header file for shm.c
*/

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdio.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include "circularBuffer.h"

#define ERROR -1

int getSharedMemoryBlock(key_t shmKey, int shmSize); 
CircularBuffer *attachSharedMemoryBlock(int shmId);
bool detachSharedMemoryBlock(CircularBuffer *shmPtr);
bool destroySharedMemoryBlock(int shmID); 

#endif