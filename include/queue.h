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
