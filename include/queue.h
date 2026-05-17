/**
 * @file queue.h
 * @brief Fixed-size byte queue implementation.
 *
 * This header defines a simple FIFO queue interface using a circular buffer.
 * The queue supports:
 * - Appending data to the tail
 * - Inserting data at the front
 * - Removing data from the head
 * - Peeking at queued data without removal
 * - Debug printing of queue contents
 */

#ifndef QUEUE_H
#define QUEUE_H

#define SIZE_QUEUE 4096

typedef struct
{
	int size;
	int tail;
	int head;
	char buffer[SIZE_QUEUE];
} queue_t;

int enqueue(queue_t *queue, unsigned char *buffer, int size);
int enqueue_front(queue_t *queue, unsigned char *buffer, int size);
int dequeue(queue_t *queue, unsigned char *buffer, int size);
int dequeue_peek(queue_t *queue, unsigned char *buffer, int size);
void print_queue(queue_t *queue);

#endif
