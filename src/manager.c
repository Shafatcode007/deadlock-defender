#include <stdio.h>
#include <unistd.h>
#include "manager.h"
#include "banker.h"

void print_state(SharedData *shm) {
    printf("\n┌─────────────────────────────────────────────────┐\n");
    printf("│              SYSTEM STATE                       │\n");
    printf("├──────────┬──────────────┬──────────────┬────────┤\n");
    printf("│ Worker   │ Allocation   │ Max          │ Need   │\n");
    printf("├──────────┼──────────────┼──────────────┼────────┤\n");
    for (int i = 0; i < NUM_WORKERS; i++) {
        printf("│ W%-7d │ [%d %d %d]      │ [%d %d %d]      │ [%d %d %d] │\n",
               i + 1,
               shm->allocation[i][0], shm->allocation[i][1], shm->allocation[i][2],
               shm->max[i][0],        shm->max[i][1],        shm->max[i][2],
               shm->need[i][0],       shm->need[i][1],       shm->need[i][2]);
    }
    printf("├──────────┴──────────────┴──────────────┴────────┤\n");
    printf("│ Available: R0=%-2d  R1=%-2d  R2=%-2d                 │\n",
           shm->available[0], shm->available[1], shm->available[2]);
    printf("└─────────────────────────────────────────────────┘\n\n");
}

/* returns 1 when every worker has set worker_done */
static int all_done(SharedData *shm) {
    for (int i = 0; i < NUM_WORKERS; i++)
        if (!shm->worker_done[i]) return 0;
    return 1;
}

void run_manager(SharedData *shm, int defender_mode) {
    printf("[Manager] PID=%d — mode: %s\n\n",
           getpid(), defender_mode ? "DEFENDER" : "VULNERABLE");

    if (!defender_mode) {
        /*
         * Vulnerable mode: workers self-grant without any check.
         * Manager just waits for them to finish (or hang on deadlock).
         */
        printf("[Manager] WARNING: Banker's Algorithm DISABLED.\n");
        printf("[Manager] Processes may deadlock...\n\n");
        while (!all_done(shm))
            sleep(1);
        printf("\n[Manager] Vulnerable mode finished (if you see this, no hard deadlock occurred).\n");
        return;
    }

    /* Defender mode: handle requests one at a time */
    print_state(shm);

    while (!all_done(shm)) {
        /* block until a worker posts a request */
        sem_wait(&shm->request_ready);

        sem_wait(&shm->mutex);

        int owner = shm->request_owner;
        int req[NUM_RESOURCES];
        for (int j = 0; j < NUM_RESOURCES; j++)
            req[j] = shm->request[j];

        printf("[Manager] Request from Worker %d: R0=%d R1=%d R2=%d\n",
               owner + 1, req[0], req[1], req[2]);

        /* run Banker's Algorithm — modifies shm if granted */
        int granted = banker_request(shm, owner, req);

        shm->request_result = granted;
        shm->request_owner  = -1; /* clear before releasing mutex */

        sem_post(&shm->mutex); /* release mutex before printing (non-blocking) */

        if (granted == 1)
            printf("[Manager] Decision: GRANTED — system still SAFE\n");
        else if (granted == -1)
            printf("[Manager] Decision: DENIED  — exceeds declared maximum need\n");
        else if (granted == -2)
            printf("[Manager] Decision: DENIED  — insufficient resources available\n");
        else
            printf("[Manager] Decision: DENIED  — would lead to unsafe state\n");

        sem_wait(&shm->mutex);
        print_state(shm);
        sem_post(&shm->mutex);

        /* wake the waiting worker */
        sem_post(&shm->request_done);
    }

    printf("[Manager] All workers finished. No deadlock occurred.\n");
    print_state(shm);
}

void run_manual_mode(SharedData *shm) {
    printf("\n[Manual Mode] Banker's Algorithm ON. Type requests interactively.\n");
    printf("[Manual Mode] Worker IDs: 1–%d  |  Resources: R0 R1 R2\n\n", NUM_WORKERS);

    print_state(shm);

    int worker_id, req[NUM_RESOURCES];

    while (1) {
        printf("Enter Worker ID (1-%d) or 0 to exit: ", NUM_WORKERS);
        if (scanf("%d", &worker_id) != 1) break;
        while (getchar() != '\n'); /* flush newline */
        if (worker_id == 0) break;

        if (worker_id < 1 || worker_id > NUM_WORKERS) {
            printf("[Manual Mode] Invalid worker ID.\n");
            continue;
        }

        printf("Enter request (R0 R1 R2): ");
        if (scanf("%d %d %d", &req[0], &req[1], &req[2]) != 3) break;
        while (getchar() != '\n'); /* flush newline */

        int i = worker_id - 1; /* 0-based */

        printf("\n[Manager] Request from Worker %d: R0=%d R1=%d R2=%d\n",
               worker_id, req[0], req[1], req[2]);

        /* reuse existing Banker's Algorithm — same function used by defender mode */
        int granted = banker_request(shm, i, req);

        if (granted == 1)
            printf("[Manager] Decision: GRANTED — system still SAFE\n");
        else if (granted == -1)
            printf("[Manager] Decision: DENIED  — exceeds declared maximum need\n");
        else if (granted == -2)
            printf("[Manager] Decision: DENIED  — insufficient resources available\n");
        else
            printf("[Manager] Decision: DENIED  — would lead to unsafe state\n");

        print_state(shm);
    }

    printf("[Manual Mode] Exiting.\n");
}
