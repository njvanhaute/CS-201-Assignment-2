/****************************************************************\
 * FILE: queue.h
 * AUTHOR: Nicholas Vanhaute
 * COURSE: CS 201
 * PROFESSOR: Dr. Lusth
 * ASSIGN0
 * This is the header file for the queue module. It's based upon
 *     the circular dynamic array module.
 \****************************************************************/

#ifndef __QUEUE_INCLUDED__
#define __QUEUE_INCLUDED__

#include <stdio.h>

typedef struct queue QUEUE;

extern QUEUE *newQUEUE(void (*d)(FILE *,void *)); 
extern void enqueue(QUEUE *items,void *value);
extern void *dequeue(QUEUE *items);
extern void *peekQUEUE(QUEUE *items);
extern int sizeQUEUE(QUEUE *items);
extern void displayQUEUE(FILE *,QUEUE *items);
extern void visualizeQUEUE(FILE *,QUEUE *items);

#endif
