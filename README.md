# HISTO-SYSTEM

## Overview
The HISTO-SYSTEM consists of three distinct processing components: a "data consumer" (`DC`) application, and two different "data producer" (`DP-1` and `DP-2`) applications. The data producers write a letter to the to a circular buffer and the data consumer reads from the circular buffer at the same time and prints a histogram every 10 seconds.

## Components
The system consists of three main components:
* **Data Processer-1(`DP-1`)** - The `DP-1` writes data to a circular buffer every 2 seconds. The circular buffer is attached to a shared memory and is accessible by `DP-2` and `DC` 
* **Data Processer-1(`DP-2`)** - The `DP-2` writes data to the same circular buffer every 1/20 seconds.
* **Data Consumer (`DC`)** - The `DC` reads from the circular buffer every 2 seconds and prints a histogram

## Compiling
To run the HISTO-SYSTEM, follow these steps:
1. Compile the overall system using the following command
```bash
make all
```
2. To compile each system separately, you can type `make` while in the `DP-1`, `DP-2` or `DC`.

## Running the HISTO-SYSTEM
1. Open the terminal inside the `HISTO-SYSTEM` 
2. You can run the `DP-1` using the following command
```bash
./DP-1/bin/dp1
```
3. You can run the `DP-2` using the following command
```bash
./DP-2/bin/dp2
```
4. You can run the `DC` using the following command
```bash
./DC/bin/dc
```

## Cleaning the file structure
* You can clean the folder using 
```bash
make clean
```
* You can clean the log folder using
```bash
make clean_log
```

## File Structure
- `DP-1/`: Contains the files for the `DP-1`
- `DP-2/`: Contains the files for the `DP-2`
- `DC/`: Contains the files for the `DC`
- `common/`: Stores files use by both `DP-1`,`DP-1` and `DC` 
- `src/`: Contains the source code files
- `inc/`: Contains header files.
- `obj/`: Stores object files 
- `bin/`: Stores executable 
- `tmp/`: Contains the log files

## The `/Common` folder
* The `/Common` folder contains the files that are used both all 3 systems:
  - `semaphore.c` and `semaphore.h` contains the basic functions for creating and using a semaphore
  - `circularBuffer.c` and `circularBuffer.h` contains the basic functions for creating the circular buffer. Such as writing to the circular buffer, reading from etc
  - `logger.c` and `logger.h` contains the basic logging functions
  - `shm.c` and `shm.c` contain the basic functions for using a shared memory

## Authors
- Abraham Vijai 

