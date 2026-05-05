#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "worker.h"

/*
 * Predefined request scenarios per worker.
 * Each worker makes 2 sequential requests.
 * Chosen to trigger Banker's denial in defender mode.
 */
static int scenarios[NUM_WORKERS][2][NUM_RESOURCES] = {
    /* Worker 1 */ {{0,1,0}, {2,0,0}},
    /* Worker 2 */ {{1,0,0}, {0,1,1}},
    /* Worker 3 */ {{0,0,1}, {1,0,0}},
};

static void send_request(int id, SharedData *shm, int req[]) {
    /* write request vector into shared memory */
    sem_wait(&shm->mutex);
    for (int j = 0; j < NUM_RESOURCES; j++)
        shm->request[j] = req[j];
    shm->request_owner  = id - 1; /* 0-based */
    shm->request_result = -1;     /* pending */
    sem_post(&shm->mutex);

    /* wake manager */
    sem_post(&shm->request_ready);

    /* block until manager decides */
    sem_wait(&shm->request_done);
}

static void self_grant(int id, SharedData *shm, int req[]) {
    /* vulnerable mode: skip manager, allocate directly */
    sem_wait(&shm->mutex);
    int i = id - 1;
    for (int j = 0; j < NUM_RESOURCES; j++) {
        shm->allocation[i][j] += req[j];
        shm->available[j]     -= req[j];
        shm->need[i][j]       -= req[j];
    }
    sem_post(&shm->mutex);
}

static void release_all(int id, SharedData *shm) {
    sem_wait(&shm->mutex);
    int i = id - 1;
    printf("[Worker %d] Releasing all resources\n", id);
    for (int j = 0; j < NUM_RESOURCES; j++) {
        shm->available[j]      += shm->allocation[i][j];
        shm->allocation[i][j]   = 0;
        shm->need[i][j]         = shm->max[i][j];
    }
    shm->worker_done[i] = 1;
    sem_post(&shm->mutex);
}

void run_worker(int id, SharedData *shm, int defender_mode) {
    printf("[Worker %d] PID=%d started\n", id, getpid());

    for (int round = 0; round < 2; round++) {
        int *req = scenarios[id - 1][round];

        printf("[Worker %d] Requesting R0=%d R1=%d R2=%d\n",
               id, req[0], req[1], req[2]);

        if (defender_mode) {
            send_request(id, shm, req);

            /* read result */
            sem_wait(&shm->mutex);
            int result = shm->request_result;
            sem_post(&shm->mutex);

            if (result == 1)
                printf("[Worker %d] >> GRANTED\n", id);
            else if (result == -1)
                printf("[Worker %d] >> DENIED (exceeds declared maximum need)\n", id);
            else if (result == -2)
                printf("[Worker %d] >> DENIED (insufficient resources available)\n", id);
            else
                printf("[Worker %d] >> DENIED (would lead to unsafe state)\n", id);
        } else {
            /* vulnerable: no safety check */
            self_grant(id, shm, req);
            printf("[Worker %d] >> self-granted (no check)\n", id);
        }

        sleep(1); /* simulate using resource */
    }

    release_all(id, shm);
    printf("[Worker %d] Done. Exiting.\n", id);
    exit(0);
}
