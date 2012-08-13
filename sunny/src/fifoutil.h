/**
 * @file    fifoutil.h
 * @brief
 *
 * This interface enables easy passing of fixed size messages between POSIX
 * threads in Linux using first in first out ordering. Only one reader and
 * writer per fifo is supported unless the application serializes the calls.
 *
 * @verbatim
 * =========================================================================== * Copyright (c) Texas Instruments Inc 2005
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied or provided.
 * =========================================================================== * @endverbatim
 */

#ifndef _FIFOUTIL_H
#define _FIFOUTIL_H

#include <unistd.h>
#include <errno.h>

/**
 * @brief Object that stores the state of the fifo.
 */
typedef struct Fifo_Obj {
    size_t size;
    int pipes[2];
} Fifo_Obj;

/**
 * @brief Pointer to a fifo util object
 */
typedef Fifo_Obj *Fifo_Handle;

/**
 * @brief Indicates that a fifo util API call completed successfully.
 */
#define FIFO_SUCCESS 0

/**
 * @brief Indicates that a fifo util API call failed.
 */
#define FIFO_FAILURE -1

int Fifo_open(Fifo_Handle hFifo, size_t size);
int Fifo_close(Fifo_Handle hFifo);
int Fifo_get(Fifo_Handle hFifo, void *buffer);
int Fifo_put(Fifo_Handle hFifo, void *buffer);
int Fifo_get_wsize(Fifo_Handle hFifo, void *buffer, size_t size);
int Fifo_put_wsize(Fifo_Handle hFifo, void *buffer, size_t size);
int Fifo_getInputFd(Fifo_Handle hFifo);
int Fifo_getOutputFd(Fifo_Handle hFifo);
int Fifo_flush(Fifo_Handle hFifo);

#endif				

