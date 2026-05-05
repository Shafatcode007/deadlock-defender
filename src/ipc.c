#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include "ipc.h"

/* starting available units for each resource */
static int init_available[NUM_RESOURCES] = {10, 5, 7};

/* maximum demand declared by each worker at start */
static int init_max[NUM_WORKERS][NUM_RESOURCES] = {
    {7, 5, 3},   /* Worker 1 */
    {3, 2, 2},   /* Worker 2 */
    {9, 0, 2},   /* Worker 3 */
};

SharedData *ipc_init(void) {
    /* allocate anonymous shared memory segment */
    int shmid = shmget(IPC_PRIVATE, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("[IPC] shmget failed");
        exit(1);
    }

    /* attach segment to this process's address space */
    SharedData *shm = (SharedData *)shmat(shmid, NULL, 0);
    if (shm == (SharedData *)-1) {
        perror("[IPC] shmat failed");
        exit(1);
    }

    /* zero the whole structure first */
    memset(shm, 0, sizeof(SharedData));

    /* fill available resources */
    for (int j = 0; j < NUM_RESOURCES; j++)
        shm->available[j] = init_available[j];

    /* fill max, compute need = max - 0 = max, allocation starts at 0 */
    for (int i = 0; i < NUM_WORKERS; i++) {
        for (int j = 0; j < NUM_RESOURCES; j++) {
            shm->max[i][j]        = init_max[i][j];
            shm->need[i][j]       = init_max[i][j];
            shm->allocation[i][j] = 0;
        }
        shm->worker_done[i] = 0;
    }

    shm->request_owner  = -1;
    shm->request_result = -1;

    /* pshared=1 so semaphores work across fork() */
    sem_init(&shm->mutex,         1, 1);
    sem_init(&shm->request_ready, 1, 0);
    sem_init(&shm->request_done,  1, 0);

    printf("[IPC] Shared memory ready. Available: R0=%d R1=%d R2=%d\n",
           shm->available[0], shm->available[1], shm->available[2]);

    return shm;
}

void ipc_cleanup(SharedData *shm) {
    sem_destroy(&shm->mutex);
    sem_destroy(&shm->request_ready);
    sem_destroy(&shm->request_done);
    shmdt(shm);
    printf("[IPC] Shared memory detached and semaphores destroyed.\n");
}
