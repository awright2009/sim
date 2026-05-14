/**
* @file queue.cpp
* @brief Circular queue implementation for byte-based data storage.
*/

#include "queue.h"

#include <stdio.h>


/**
* @brief Add data to the back of the queue.
*
* Inserts bytes from the provided buffer into the queue tail
* until either all data is inserted or the queue becomes full.
*
* @param queue Pointer to queue structure.
* @param buffer Pointer to source data buffer.
* @param size Number of bytes to enqueue.
*
* @return Number of bytes successfully enqueued.
*/
int enqueue(queue_t *queue, unsigned char *buffer, int size)
{
	int        i;

	if (queue->size == SIZE_QUEUE)
	{
		return 0;
	}

	for (i = 0; i < size && queue->size != SIZE_QUEUE; i++)
	{
		queue->buffer[queue->tail++] = buffer[i];
		queue->size++;
		if (queue->tail == SIZE_QUEUE)
			queue->tail = 0;
	}
	return i;
}


/**
* @brief Add data to the front of the queue.
*
* Inserts bytes from the provided buffer into the queue head
* in reverse order so that the original ordering is preserved
* when dequeued.
*
* @param queue Pointer to queue structure.
* @param buffer Pointer to source data buffer.
* @param size Number of bytes to enqueue.
*
* @return Number of bytes successfully enqueued.
*/
int enqueue_front(queue_t *queue, unsigned char *buffer, int size)
{
	int        i;

	if (queue->size == SIZE_QUEUE)
		return 0;

	for (i = 0; i < size && queue->size != SIZE_QUEUE; i++)
	{
		if (queue->head == 0)
		{
			queue->head = SIZE_QUEUE;
		}
		queue->buffer[--queue->head] = buffer[(size - 1) - i];
		queue->size++;
	}
	return i;
}


/**
* @brief Remove data from the front of the queue.
*
* Copies bytes from the queue into the destination buffer
* and advances the queue head position.
*
* Removed queue entries are overwritten with 255
* for debugging visibility.
*
* @param queue Pointer to queue structure.
* @param buffer Pointer to destination buffer.
* @param size Maximum number of bytes to dequeue.
*
* @return Number of bytes successfully dequeued.
*/
int dequeue(queue_t *queue, unsigned char *buffer, int size)
{
	int        i;

	for (i = 0; i < size && queue->size != 0; i++)
	{
		buffer[i] = queue->buffer[queue->head++];
		queue->buffer[queue->head - 1] = 255; // empty space for debugging
		queue->size--;
		if (queue->head == SIZE_QUEUE)
		{
			queue->head = 0;
		}
	}
	return i;
}

/**
* @brief Peek at queue data without removing it.
*
* Copies bytes from the queue into the destination buffer
* without modifying the queue state, head pointer,
* or queue size.
*
* @param queue Pointer to queue structure.
* @param buffer Pointer to destination buffer.
* @param size Maximum number of bytes to peek.
*
* @return Number of bytes successfully copied.
*/
int dequeue_peek(queue_t *queue, unsigned char *buffer, int size)
{
	int        i;
	int		head = queue->head;
	int		qsize = queue->size;

	for (i = 0; i < size && qsize != 0; i++)
	{
		buffer[i] = queue->buffer[head++];
		//queue->buffer[queue->head - 1] = ' '; // empty space for debugging
		qsize--;
		if (head == SIZE_QUEUE)
		{
			head = 0;
		}
	}
	return i;
}


/**
* @brief Print the current queue contents and state.
*
* Displays all queue entries in logical order starting
* from the queue head, followed by queue metadata
* including head index, tail index, and current size.
*
* @param queue Pointer to queue structure.
*/
void print_queue(queue_t *queue)
{
	int i;
	int index = queue->head;

	printf("\tCharger queue: ");

	for (i = 0; i < queue->size; i++)
	{
		printf("%d ", (int)queue->buffer[index]);

		index++;
		if (index == SIZE_QUEUE)
			index = 0;
	}

	printf("head %d tail %d size %d\n",
		queue->head,
		queue->tail,
		queue->size);
}

