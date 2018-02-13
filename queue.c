#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "cda.h"
#include "queue.h"

struct queue {
    CDA *cda;
    void (*display)(FILE *, void *);
};

// Function: newQUEUE
// Takes in a function pointer to display the type of object stored.
// Returns the newly allocated queue.

QUEUE *newQUEUE(void (*d)(FILE *, void *)) {
    QUEUE *queue = (QUEUE *)malloc(sizeof(QUEUE));
    assert(queue != 0);
    queue->cda = newCDA(d);
    queue->display = d;
    return queue;
}

// Function: enqueue
// Takes in a queue object and a void pointer.
// Enqueues the void pointer

void enqueue(QUEUE *items, void *value) {
    insertCDAback(items->cda, value);
}

// Function: dequeue
// Takes in a queue object
// Dequeues and returns the object at the front of the queue

void *dequeue(QUEUE *items) {
    assert(sizeCDA(items->cda) > 0);
    return removeCDAfront(items->cda);
}

// Function: peekQUEUE
// Takes in a queue object
// Returns but does not dequeue the object at the front of the queue.

void *peekQUEUE(QUEUE *items) {
    assert(sizeCDA(items->cda) > 0);
    return getCDA(items->cda, 0);
}

// Function: sizeQUEUE
// Takes in a queue object
// Returns the number of items in the queue.

int sizeQUEUE(QUEUE *items) {
    return sizeCDA(items->cda);
}

// Function: displayQUEUE
// Takes in a file pointer and a queue object
// Prints the elements of the queue enclosed by angle brackets and separated
//      by commas.

void displayQUEUE(FILE *fp, QUEUE *items) {
    fprintf(fp, "<");
    for (int i = 0; i < sizeQUEUE(items); i++) {
        items->display(fp, getCDA(items->cda, i));
        if (i < sizeQUEUE(items) - 1) {
            fprintf(fp, ",");
        }
    }
    fprintf(fp, ">");
}

// Function: visualizeQUEUE
// Takes in a file pointer and a queue object.
// Calls the display function of the underlying data structure (CDA)
void visualizeQUEUE(FILE *fp, QUEUE *items) {
    displayCDA(fp, items->cda);
}
