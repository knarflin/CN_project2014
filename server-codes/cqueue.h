// cqueue.h
// circular queue

#include "job.h"

#ifndef CQUEUE
#define CQUEUE

#define MAX_QUEUE_SIZE 100

struct cqueue{
	int head;
	int tail;
	struct job* jobarray[MAX_QUEUE_SIZE];
};

int enqueue(struct cqueue* cq,struct job* elem){
	if(cq->head==(cq->tail+1)%MAX_QUEUE_SIZE)
		return -1; // queue is full
	cq->jobarray[cq->tail]=elem;
	cq->tail=(cq->tail+1)%MAX_QUEUE_SIZE;
	return 0;
}

// return -1 for empty queue, 0 for successful
int dequeue(struct cqueue* cq,struct job** elem){
	if(cq->head==cq->tail)
		return -1;
	if(elem!=NULL)
		*elem=cq->jobarray[cq->head];
	cq->head=(cq->head+1)%MAX_QUEUE_SIZE;
	return 0;
}

void print_queue(struct cqueue* cq){ // just for debugging
	int i;
	for(i=cq->head; i!=cq->tail; i=(i+1)%MAX_QUEUE_SIZE){
		printf("---Job at position %d:---\n",i);
		print_job(cq->jobarray[i]);
	}
}

#endif