#ifndef IPC_H
#define IPC_H

#include <semaphore.h>

#define NUM_WORKERS   3
#define NUM_RESOURCES 3

/* shared memory layout — all processes read/write this struct */
typedef struct {
    int available[NUM_RESOURCES];               /* free resource units */
    int allocation[NUM_WORKERS][NUM_RESOURCES]; /* held by each worker */
    int max[NUM_WORKERS][NUM_RESOURCES];        /* max ever needed */
    int need[NUM_WORKERS][NUM_RESOURCES];       /* max - allocation */

    int request[NUM_RESOURCES]; /* pending request vector */
    int request_owner;          /* worker index (0-based), -1 = none */
    int request_result;         /* 1=granted, 0=denied, -1=pending */

    int worker_done[NUM_WORKERS]; /* 1 when worker has finished */

    sem_t mutex;          /* protects whole struct */
    sem_t request_ready;  /* worker → manager: request waiting */
    sem_t request_done;   /* manager → worker: decision made */
} SharedData;

/* create + initialise shared memory, return pointer */
SharedData *ipc_init(void);

/* detach and destroy semaphores */
void ipc_cleanup(SharedData *shm);

#endif
