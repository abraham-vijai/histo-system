/*
Filename    : circularBuffer.h
Author      : Abraham Vijai
Date        : 2024-04-09
Description : Header file for circularBuffer.c
*/

#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFFER_SIZE 256
#define ERROR -1

const char letters[20] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 
                        'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T'};

typedef struct {
    char buffer[BUFFER_SIZE];
    int readIndex;
    int writeIndex;
    int size;
} CircularBuffer;

CircularBuffer *createCircularBuffer();
bool isCircularBufferFull(CircularBuffer *cb);
int writeBuffer(CircularBuffer *cb, int value);
char readBuffer(CircularBuffer *cb);

#endif