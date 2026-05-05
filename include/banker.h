#ifndef BANKER_H
#define BANKER_H

#include "ipc.h"

/*
 * Check if current system state is safe.
 * Returns 1 if safe, 0 if unsafe.
 */
int is_safe_state(SharedData *shm);

/*
 * Try to grant request[] for worker_id using Banker's Algorithm.
 * Returns:
 *   1  = GRANTED
 *   0  = DENIED — unsafe state
 *  -1  = DENIED — request exceeds declared max need
 *  -2  = DENIED — insufficient resources available
 */
int banker_request(SharedData *shm, int worker_id, int request[]);

#endif
