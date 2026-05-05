#ifndef WORKER_H
#define WORKER_H

#include "ipc.h"

/*
 * Entry point for a worker child process.
 * id           : 1-based worker number
 * shm          : pointer to shared memory
 * defender_mode: 1 = use manager, 0 = self-grant (vulnerable)
 * Never returns — calls exit().
 */
void run_worker(int id, SharedData *shm, int defender_mode);

#endif
