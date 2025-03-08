/*
Filename    : semaphore.h
Author      : Abraham Vijai
Date        : 2024-04-09
Description : Header file for semaphore.c
*/ 

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR -1

struct sembuf acquire_operation = { 0, -1, SEM_UNDO };
struct sembuf release_operation = { 0, 1, SEM_UNDO };
unsigned short init_values[1] = { 1 };

int getSemaphore();
int initSemaphore();
int lockSemaphore();
int releaseSemaphore();
int destroySemaphore();

#endif 
