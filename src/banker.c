#include <stdio.h>
#include "banker.h"

/*
 * Safety Algorithm (Dijkstra):
 *   work   = copy of available
 *   finish = all false
 *   Repeatedly find process i where:
 *       finish[i]==0 AND need[i] <= work
 *   If found: work += allocation[i], finish[i] = true
 *   If all finish[] == true → SAFE, else UNSAFE
 */
int is_safe_state(SharedData *shm) {
    int work[NUM_RESOURCES];
    int finish[NUM_WORKERS];

    for (int j = 0; j < NUM_RESOURCES; j++)
        work[j] = shm->available[j];

    for (int i = 0; i < NUM_WORKERS; i++)
        finish[i] = 0;

    int progress = 1;
    while (progress) {
        progress = 0;
        for (int i = 0; i < NUM_WORKERS; i++) {
            if (finish[i]) continue;

            /* can worker i finish with current work? */
            int fits = 1;
            for (int j = 0; j < NUM_RESOURCES; j++) {
                if (shm->need[i][j] > work[j]) {
                    fits = 0;
                    break;
                }
            }

            if (fits) {
                /* simulate worker i completing and releasing */
                for (int j = 0; j < NUM_RESOURCES; j++)
                    work[j] += shm->allocation[i][j];
                finish[i] = 1;
                progress  = 1;
            }
        }
    }

    for (int i = 0; i < NUM_WORKERS; i++)
        if (!finish[i]) return 0; /* at least one stuck → unsafe */

    return 1; /* all can finish → safe */
}

/*
 * Resource-Request Algorithm:
 *   1. Validate request <= need  (worker not exceeding declared max)
 *   2. Validate request <= available  (resources exist)
 *   3. Tentatively grant
 *   4. Run safety check
 *   5. If unsafe → rollback and return 0
 */
int banker_request(SharedData *shm, int worker_id, int request[]) {
    int i = worker_id; /* 0-based */

    /* step 1: request must not exceed need */
    for (int j = 0; j < NUM_RESOURCES; j++) {
        if (request[j] > shm->need[i][j]) {
            return -1; /* exceeds declared max need */
        }
    }

    /* step 2: request must not exceed currently available */
    for (int j = 0; j < NUM_RESOURCES; j++) {
        if (request[j] > shm->available[j]) {
            return -2; /* insufficient resources */
        }
    }

    /* step 3: tentatively allocate */
    for (int j = 0; j < NUM_RESOURCES; j++) {
        shm->available[j]      -= request[j];
        shm->allocation[i][j]  += request[j];
        shm->need[i][j]        -= request[j];
    }

    /* step 4: safety check */
    if (is_safe_state(shm)) {
        return 1; /* safe — keep allocation */
    }

    /* step 5: unsafe — rollback */
    for (int j = 0; j < NUM_RESOURCES; j++) {
        shm->available[j]      += request[j];
        shm->allocation[i][j]  -= request[j];
        shm->need[i][j]        += request[j];
    }
    return 0;
}
