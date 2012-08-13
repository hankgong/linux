#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "fifoutil.h"


/**
 * @brief Opens the fifo. Must be called before other API:s on a fifo.
 * @param hFifo Pointer to the fifo object to open.
 * @param size Size in bytes of the messages to be passed through this fifo.
 * @return FIFO_SUCCESS for success or FIFO_FAILURE for failure.
 */
int Fifo_open(Fifo_Handle hFifo, size_t size)
{
    if (pipe(hFifo->pipes)) 
    {
      perror("pipe open");
      return FIFO_FAILURE;
    }

    hFifo->size = size;

    return FIFO_SUCCESS;
}

/**
 * @brief Closes the fifo. No API calls can be made on this fifo after this.
 * @param hFifo Pointer to the fifo object to close.
 * @return FIFO_SUCCESS for success or FIFO_FAILURE for failure.
 */
int Fifo_close(Fifo_Handle hFifo)
{
    int ret = FIFO_SUCCESS;

    if (close(hFifo->pipes[0])) {
	perror("pipe close 0");
	ret = FIFO_FAILURE;
    }

    if (close(hFifo->pipes[1])) {
	perror("pipe close 1");
	ret = FIFO_FAILURE;
    }

    return ret;
}

/**
 * @brief Blocking call to get a message from a fifo.
 * @param hFifo Pointer to a previously opened fifo object.
 * @param buffer A pointer to the buffer which will be copied to the fifo.
 * @return FIFO_SUCCESS for success or FIFO_FAILURE for failure.
 */
int Fifo_get(Fifo_Handle hFifo, void *buffer)
{
    if (read(hFifo->pipes[0], buffer, hFifo->size) != (int) hFifo->size) {
	perror("fifo get read");
	return FIFO_FAILURE;
    }

    return FIFO_SUCCESS;
}

inline int Fifo_get_wsize(Fifo_Handle hFifo, void *buffer, size_t size)
{
    if (read(hFifo->pipes[0], buffer, hFifo->size) != (int) size) {
	perror("fifo get read");
	return FIFO_FAILURE;
    }

    return FIFO_SUCCESS;
}

inline int Fifo_getInputFd(Fifo_Handle hFifo)
{
    return hFifo->pipes[0];
}

inline int Fifo_getOutputFd(Fifo_Handle hFifo)
{
    return hFifo->pipes[1];
}

/**
 * @brief Put a message on the fifo.
 * @param hFifo Pointer to a previously opened fifo object.
 * @param buffer A pointer to the buffer which will be copied from the fifo.
 * @return FIFO_SUCCESS for success or FIFO_FAILURE for failure.
 */
int Fifo_put(Fifo_Handle hFifo, void *buffer)
{
    if (write(hFifo->pipes[1], buffer, hFifo->size) != (int) hFifo->size) {
	perror("fifo put write");
	return FIFO_FAILURE;
    }

    return FIFO_SUCCESS;
}

int Fifo_put_wsize(Fifo_Handle hFifo, void *buffer, size_t size)
{
    if (write(hFifo->pipes[1], buffer, hFifo->size) != (int) size) {
	perror("fifo put write");
	return FIFO_FAILURE;
    }

    return FIFO_SUCCESS;
}

int Fifo_flush(Fifo_Handle hFifo)
{
    struct timeval timeoutVal;
    int iReturnValSelect;
    unsigned int FifoPipeHandle;
    fd_set selectFds;
    int iRetVal;

    unsigned char *pTemp;

    iRetVal = FIFO_SUCCESS;

    pTemp = (unsigned char *)malloc(hFifo->size);

    if (NULL == pTemp) {
	printf("Error in memory allocation");
	exit(0);
	return FIFO_FAILURE;

    }

    FifoPipeHandle = Fifo_getOutputFd(hFifo);

    while (1) {
	timeoutVal.tv_sec = 0;
	timeoutVal.tv_usec = 0;
	// clear fd set
	FD_ZERO(&selectFds);
	// add fd to the authenticate fd's list
	FD_SET(FifoPipeHandle, &selectFds);

	//Read encoded data from "NPIPE(n)" and fill it into data buffer
	// provided by Audio decoder

	iReturnValSelect = select(FifoPipeHandle + 1, &selectFds, NULL, NULL,&timeoutVal);

	if (iReturnValSelect < 0) {
	    iRetVal = FIFO_FAILURE;
	    exit(0);
	    break;

	} else if (iReturnValSelect >= 1) {

	    if (FIFO_FAILURE == Fifo_get(hFifo, pTemp)) {
		iRetVal = FIFO_FAILURE;
		exit(0);
		break;
	    }

	} else {
	    //Fifo empty
	    break;
	}
    }

    free(pTemp);
    return iRetVal;
}

