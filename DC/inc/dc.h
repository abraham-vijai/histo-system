/*
Filename    : dc.h
Author      : Abraham Vijai
Date        : 2024-04-09
Description : This is header file for DC
*/

#ifndef DC_H
#define DC_H

#include <signal.h>
#include <sys/sem.h>
#include <pthread.h>

#define NUM_LETTERS 20
#define WAKE_UP_INTERVAL 2
#define PRINT_INTERVAL 10
#define LOG_INIT_MESSAGE "============== DC started =============="
#define LOG_FILE_PATH "../DC/tmp/dc.log"
#define EXIT_STATEMENT "Shazam !!\n"

typedef struct KeyValuePair{
    char key;
    int value;
} KeyValuePair;



#endif